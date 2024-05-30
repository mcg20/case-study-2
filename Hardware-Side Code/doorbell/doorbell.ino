/* @Title: Doorbell Project
 * @Members: Leyte, Gamotia 
 * @Date 05-16-24 05:00:36 GMT +08:00
 * 
 */

// ERROR STATUS GUIDE
//CONNECTION_REFUSED (-1)
//SEND_HEADER_FAILED (-2)
//SEND_PAYLOAD_FAILED (-3)
//NOT_CONNECTED (-4)
//CONNECTION_LOST (-5)
//NO_STREAM (-6)
//NO_HTTP_SERVER (-7)
//TOO_LESS_RAM (-8)
//ENCODING (-9)
//STREAM_WRITE (-10)
//READ_TIMEOUT (-11)

#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include <queue> // data struct implementation for ease post connection
#include <Ticker.h>

// URL
const String serverUrl = "https://ap-southeast-1.aws.data.mongodb-api.com/app/data-uxjtwvh/endpoint/data/v1/action/insertOne";
const String apiKey = "OtE9RMH4EdXpMVVQHhNJh9LMpsOLogNQ4kpyzMiGAgKI35LLxVgbnAStaQUIr0Pr";

//DATABASE CREDENTIALS
const String database = "esp_doorbell"; //db
const String collection = "passerby"; // tbl
const String dataSource = "espcluster"; //tbl

//WIFI CREDENTIALS
#define _SSID "PLDTHOMEFIBR1c3f8"
#define _PASS "PLDTWIFIfx86x"

// BUTTON SETTINGS
#define DEBOUNCE_DELAY 2000 //ms [ this is to set a delay of 2 seconds to prevent spam whenever the button is pressed longer ]

const int BTN_PIN = 19;

// Variables that will change values:
int pushedTimes = 0;

int prevBtnState = HIGH; // the previous state from the input pin
int currBtnState;     // the current reading from the input pin

unsigned long lastDebounceTime = 0; // to know when was the last time the button were pressed to prevent spam when button is pressed longer
// end


WiFiClient client;
HTTPClient http;

Ticker pushToMongo; // a ticker to push the collected data from the queue into MongoDB collection
Ticker releaseRes; // resource releaser


// a way to group several related variables into one place. each variable in the structure is known as a member of the structure.
// unlike an array, a structure can contain many different data types
struct HttpRequest
{
  String data;
};

std::queue<HttpRequest> requestQueue; // initialize queue

void setup ()
{
	Serial.begin(115200);
	pinMode(BTN_PIN, INPUT_PULLUP);

	WiFi.mode(WIFI_STA);
	WiFi.begin(_SSID, _PASS);
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(1000);
		Serial.println("Connecting to WiFi...");
	}
	Serial.print("Connected to Wi-Fi network with IP Address: ");
	Serial.println(WiFi.localIP());

  http.setReuse(true);

  // attach tickers/timers
  pushToMongo.attach_ms(5000, insertToMongoDB); // pushes data collected from the queue into MongoDB every 5 sec.
  releaseRes.attach_ms(15000, releaseConn); // release connection every 15 sec.
}

void loop()
{
	currBtnState = digitalRead(BTN_PIN);
	unsigned long currTime = millis();

	if (prevBtnState == LOW && currBtnState == HIGH) // logic made to prevent spam when button is pressed longer
	{
		if ((currTime - lastDebounceTime) >= DEBOUNCE_DELAY) // made to check for every 2 seconds interval to prevent spam insertion to mongodb
		{
      pushedTimes++;
      pushData(String(pushedTimes));
		  lastDebounceTime = currTime;
		}
	}
	prevBtnState = currBtnState;

	delayMicroseconds(10); // a small delay to initialize data properly to load in their respective memory
}

void pushData(const String& data)
{
  HttpRequest newRequest;
  newRequest.data = data;

  requestQueue.push(newRequest);
  Serial.print("A new data pushed to queue: ");
  Serial.println(data);
}

/*
 * @Description: triggers every 5 seconds to push all data together that is in queue
 * when tiggered, it checks if the queue is not empty, then checks the front data of the queue, then post request, and pop. Repeats until cleared.
 */
void insertToMongoDB()
{
  while (!requestQueue.empty())
  {
    HttpRequest nextRequest = requestQueue.front();

    http.begin(serverUrl);
    http.addHeader("Connection", "Keep-Alive");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Access-Control-Request-Headers", "*");
    http.addHeader("api-key", apiKey);
    Serial.println("Connection to MongoDB has been established");

    StaticJsonDocument<200> doc;
    doc["collection"] = collection;
    doc["database"] = database;
    doc["dataSource"] = dataSource;

    JsonObject document = doc.createNestedObject("document");
    document["pressedTimes"] = nextRequest.data;

    String payload;
    serializeJson(doc, payload);

    int httpResponseCode = http.POST(payload);
    if (httpResponseCode > 0)
    {
      Serial.println("Successfully sent data to MongoDB");
      String response = http.getString();
      Serial.println("Response: ");
      Serial.println(response);
      Serial.println(payload);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }

    requestQueue.pop();
  }
}

/*
 * @Description: triggers every 5 seconds to push all data together that is in queue
 * when tiggered, it checks if the queue is not empty, then checks the front data of the queue, then post request, and pop. Repeats until cleared.
 */
void releaseConn()
{
  if (requestQueue.empty())
  {
    http.end(); // release resources after sometime
    Serial.println("Released resources");
  }
}
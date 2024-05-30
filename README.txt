* Kung mag configure mo sa settings such as database, web title, members, adto ramo sa "server_config.json"

MongoDB Installation Guide:
---------------------------
* 1. Makuha ang MongoDB URI sa https://cloud.mongodb.com/v2/66461b3140129f1306fc0952#/clusters
* 2. Connect
* 3. Drivers
* 4. Resources ---> Acces your Database Users
* 5. Edit you password to your choice

* 6. Go to Data API -> enable ( it will be used as API Endpoint to make the Microcontroller connect and insert data to database.
* 7. Go to Test Your API
* 8. Generate API Key (it will be used to the connectivity of the microcontroller to the mongodb.


Software Side Installation Guide:
---------------------------------
* 1. Install node.js at https://nodejs.org/en/download/package-manager
* 2. Create folder to any of your chosen location
* 3. Open CMD type -> cd (space)
* 4. Drag the folder prior to the folder you created, CMD wil something look like this: cd C:\xampp\Your FolderName
* 5. Enter, then type -> npm init -y
* 6. After, then type -> npm install express mongodb ejs
* 7. Go to server_config.js [config your data according to your mongodb account]
* 8. After configuration you may now type: npm start
* 9. Access your dashboard at http://127.0.0.1:3000/


© Paklay
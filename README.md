# Smart-Parking-Lot-using-Digital-Twin

# Step 1: Sensor Selection and Installation

Take ultrasonic HC-SR04 for distance calculation.
Strategically plan sensor placement for optimal coverage.
Precisely calibrate sensor installation for accurate distance measurements.



# Step 2: Data Collection and Integration

Run 'smartpl.ino' in Arduino IDE and continuously collect real-time data from installed sensors.
Edit the secret.h file with your wifi credentials and AWS service credentials.
Establish protocols for ongoing monitoring to ensure system responsiveness.



# Step 3: AWS IoT Core Integration

Set up AWS IoT Core to manage and communicate with IoT devices securely.
Create a thing, policy, and view incoming real-time data through MQTT Test Client.



# Step 4: Data Transmission to AWS IoT Core

Publish data to AWS IoT Core using MQTT topics.
View real-time data transmission in the MQTT Test Client.



# Step 5: AWS Sitewise Integration

Set up message routing rules to transmit data from AWS IoT to AWS IoT Sitewise.
Create models, assets, and link them with IoT Core data in AWS Sitewise.



# Step 6: Model Creation

Use Blender to create a 3D model of the parking lot.
Import the model as .glb into AWS TwinMaker and establish rules based on real-time data.




# Step 7: Visualization in Grafana

Create a workspace in Grafana and add TwinMaker as the data source.
Visualize real-time data in the Grafana dashboard.


* node-red

bash <(curl -sL https://raw.githubusercontent.com/node-red/linux-installers/master/deb/update-nodejs-and-nodered) --node18

sudo apt-get install mosquitto mosquitto-clients

Pakete:
node-red-dashboard
ui_led
node-red-contrib-controltimer
node-red-contrib-ui-time-scheduler
node-red-contrib-ui-actions

* mosquitto

remove retained messages, save state

sudo systemctl stop mosquitto
sudo rm /var/lib/mosquitto/mosquitto.db
sudo systemctl start mosquitto

* node-red

.node-red/settings

contextStorage: {
   default: "memoryOnly",
   memoryOnly: { module: 'memory' },
   file: { module: 'localfilesystem' }
},

TODO: add auth

pi@raspberrypi:~ $ 

cat .node-red/context/global/global.json 
{
    "sprinklerDuration": 21
}

cat .node-red/context/4eb00aa7fe93676d/04dd8f1d89d7454a.json 
{
    "timers": [

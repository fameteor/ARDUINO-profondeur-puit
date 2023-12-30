


const CellarMeasures  = new Mongo.Collection('cellarMeasures')


// Get measures in the database -------------------------------------
const { SerialPort } = require('serialport')
const port =  new SerialPort({path: '/dev/rfcomm0', baudRate: 56000});
const bound = Meteor.bindEnvironment((callback) => {callback()})
port.on('readable', function () {
bound(function() {
  try {
	  var data = JSON.parse(port.read().toString());
	  // We add a date stamp
	  const mesureDate = new Date(data.d[0], data.d[1] - 1, data.d[2],data.d[3],data.d[4], data.d[5]);
	  console.log(mesureDate);
	  data.date = mesureDate;
	  console.log(CellarMeasures.insert(data));
	  
	  console.log("Mesure " + data.n + " du " + data.d[2] + "/" + data.d[1] + "/" + data.d[0] + " à "  + data.d[3] + ":" + data.d[4] + ":" + data.d[5]);
	  console.log("- température : " + data.t + " ° C");
	  console.log("- humidité : " + data.h + " %");
	  console.log("- hauteur d'eau : " + data.x + " cm");
	  console.log("    - variation 30mn : " + data.v[0] + " cm");
	  console.log("    - variation 1h : " + data.v[1] + " cm");
	  console.log("    - variation 3h : " + data.v[2] + " cm");
	  console.log("    - variation 6h : " + data.v[3] + " cm");
	  console.log("--------------------------------------------------");
	
 	} catch (e) {
  		console.log(e);
	}
}) });

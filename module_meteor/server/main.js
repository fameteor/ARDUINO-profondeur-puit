


const CellarMeasures  = new Mongo.Collection('cellarMeasures')


// Get the cave.txt data --------------------------------------------
try {
	const sdData = Assets.getText('CAVE.TXT');
	const sdDateLines = sdData.split('\n');
	console.log("Loading CAVE.TXT data -------------------------------- ");
	console.log("nb de lignes : " + sdDateLines.length);
	sdDateLines.forEach(function(line) {
		try {
		  var data = JSON.parse(line);
		  // We add a date stamp
		  const mesureDate = new Date(data.d[0], data.d[1] - 1, data.d[2],data.d[3],data.d[4], data.d[5]);
		  data.date = mesureDate;
		  CellarMeasures.insert(data);	
	 	} catch (e) {
	  		// No JSON data on this line
	  		console.log("power " + line);
		}

	});
	console.log("Loading done ---------------------------------------- ");
} catch (e) {
	console.log("no CAVE.TXT file ");
}


// Put measures in the database -------------------------------------
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

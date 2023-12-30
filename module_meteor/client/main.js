const CellarMeasures  = new Mongo.Collection('cellarMeasures');

const twoDigits = function(value) {
	if (value < 10) return "0" + value;
	else return String(value);
}


const drawChart = function() {
  var data = CellarMeasures.find({}, {sort: {date: -1}}).fetch();
  if (data.length === 0) {
    console.log("Aucune donnée disponible pour cette sélection");
  }
  else {
    // Draw chart
    MG.data_graphic({
	// DATA ----------------------------------------
	data: data,
	title: 			"niveau d'eau du puit de Fort Barriat",
	target: 		document.getElementById('chart'),
	x_accessor: 		'date',
	y_accessor: 		['x'],
	// description: 	"Vous pouvez sélectionner une zone avec la souris pour zoomer.",
	
	// AXES ----------------------------------------
	// - Cadrillage vertical des marques X et y et nb de marques
	x_extended_ticks : 	true,
	xax_count : 		10,
	y_extended_ticks : 	true,
	yax_count : 		8,
	// - Affichage des unité sur l'axe Y en suffixe
	yax_units:		" cm", 
	yax_units_append: 	true,
	// - Axe X sur 24h
	// european_clock : 	true, // Bug : affichage en double sur l'axe X
	legend:			["hauteur"],
	// Commencer l'axe y aux valeurs les plus basses
	min_y_from_data: true,
	// max_y:500,
	// min_y:0,
	yax_format: function(d) { return d + " cm"; },
	
	// GRAPHICS ------------------------------------
	// - Couleurs des séries
	color: 				['blue'],
	// - Pour une image resisable
	full_width: 		true,
	// - Interpolation linéaire
	interpolate: 		d3.curveLinear,
	// - Pour le zoom
	brush: 				'x',
	
	// LAYOUT --------------------------------------
	// - Hauteur de la zone complète
	height: 			500,
	// - Marge à droite et à gauche pour échelle, légende de la série
	top:				50,
	right: 				0,
	left: 				100,
	
	// Show missing observations as missing line segments
	//missing_is_hidden: true,
	
	// Pour n'avoir que le trait : false
	area: true,
	
	// Localisation
	/*
	xax_format: function(d) {
		var locale = d3.locale(loc);
		var df = locale.timeFormat('%b %d');
		return df(d);
	},
	*/
	
	// Marche pas
	// show_secondary_x_label :true,
	// error:			"Pas de données disponible",
	// missing_text:		"Pas de données disponibles",
  });
  }
}


Template.lastValue.helpers({
	"draw": function() {
	drawChart();
	},
	"essai" : function() {

		return CellarMeasures.findOne({}, {sort: {date: -1, limit: 1}});
	},
	"date" : function() {
	
		return twoDigits(this.d[2]) + ":" + twoDigits(this.d[1]) + ":" + twoDigits(this.d[0]);
	},
	"heure" : function() {
	
		return twoDigits(this.d[3]) + ":" + twoDigits(this.d[4]) + ":" + twoDigits(this.d[5]);
	},
	
});

Template.lastValue.events = {
/*
	'keyup #choix_doc_titre': function (e,tpl) {
		e.preventDefault();
		// On modifie des données sessions en cas de frappe pour le titre
		Session.set('choix_doc_titre', tpl.find("#choix_doc_titre").value);
	}
*/
};

Template.lastValue.rendered = function() {
  drawChart();
}

const CellarMeasures  = new Mongo.Collection('cellarMeasures')


Template.lastValue.helpers({
	"essai" : function() {
	
	/*
		const ctx = document.getElementById('myChart');
  
  const donnees = CellarMeasures.find().fetch().map((value) => value.x);
  console.log(donnees);

	if (donnees.length > 0) {
	  new Chart(ctx, {
	    type: 'line',
	    data: {
	      // labels: ['Red', 'Blue', 'Yellow', 'Green', 'Purple', 'Orange'],
	      datasets: [{
		label: 'Hauteur d\'eau',
		data: donnees,
		borderWidth: 1,
	      }]
	    },
	    options: {
	      scales: {
		y: {
		  beginAtZero: false
		}
	      }
	    }
	  });
	}
	*/
	console.log("done");
		return CellarMeasures.findOne({}, {sort: {n: -1, limit: 1}});
	},
	"date" : function() {
	
		return this.d[2] + "/" + this.d[1] + "/" + this.d[0];
	},
	"heure" : function() {
	
		return this.d[3] + ":" + this.d[4] + ":" + this.d[5];
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
	


}

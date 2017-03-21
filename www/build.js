var results ={};

var label_to_id = {};
var label_to_bin = {};

//----------------------------------------------------------------------------------------
// distance matrix
function hits_as_dist(D)
{
	var html = '';
	
	
	var n = D.length;

	for (var i = 1; i < n; i++)
	{
		html += '';
		
		for (var j = 0; j < i; j++)
		{
			html += D[i][j] + ' ';
		}
		
		html += '<br />';
	}
	
	$('#matrix').html(html);

}



function buildtree(results)
{

	// Number of sequences
	var n = results.hits.hits.length;

	var cp = 
		cartesianProduct(
			[
				['A','C','G','T'], 
				['A','C','G','T'], 
				['A','C','G','T'], 
				['A','C','G','T'], 
				['A','C','G','T']
			]
		);

	//document.write(JSON.stringify(cp));

	// encode sequences as 5-tuples
	var tuples = [];

	for (var i = 0; i < n; i++) 
	{
		tuples[i] = {};
	
		// intialise
		for (var j in cp)
		{		
			tuples[i][cp[j].join("")] = 0;
		}

		var seq = results.hits.hits[i]._source.consensusSequence;
		var len = seq.length;
	
		for (var j = 0; j < len - 5; j++)
		{
			var tuple = seq.substring(j, j + 5);
			//document.write(tuple + '<br/>');
		
			if (tuple in tuples[i]) {
				tuples[i][tuple]++;
			}
		}
	}

	// labels
	var taxa = [];

	for (var i = 0; i < n; i++) {
		var label = results.hits.hits[i]._source.materialSampleID;

		label = label.replace(".COI-5P", "");
		label = label.replace("-", "");
	
		if (results.hits.hits[i]._source.scientificName) {
			label += ' ' + results.hits.hits[i]._source.scientificName;
			label = label.replace(/\s+/g, "_");
			label = label.replace(/\(/g, "_");
			label = label.replace(/\)/g, "_");
			label = label.replace(":", "");
		}
		
		label_to_id[label] = i;
		
		label_to_bin[label] = results.hits.hits[i]._source.taxonID;
	
		taxa.push({name:label});
	}
	
	for (var i in label_to_id) {
		console.log(i + ' ' + label_to_id[i]);
	}

	//document.write(JSON.stringify(taxa, null, 2));

	// compute distances
	var D = [];
	for (var i = 0; i < n; i++) {
		D[i] = [];
		for (var j = 0; j < n; j++) {
			D[i][j] = 0;
		}
	}

	

	for (var i = 1; i < n; i++) {
		for (var j = 0; j < i; j++) {
			var d = 0;
			// compare tuples
			for (var k in tuples[i]) {
				d += (tuples[i][k] - tuples[j][k]) * (tuples[i][k] - tuples[j][k]);
			}
		
			D[i][j] = d;
			D[j][i] = d;
		}
	}
	
	//hits_as_dist(D);
	
	// note that tree buildign will destroy values in matrix D, so if
	// we wnt it beyond here we will need to make a copy




	//document.write(JSON.stringify(D, null, 2));

	var RNJ = new RapidNeighborJoining(D, taxa);
	RNJ.run();
	console.log('run');
	var treeObject = RNJ.getAsObject();
	var treeNewick = RNJ.getAsNewick();
	console.log(treeNewick);

	showtree('svg', treeNewick);
	
	return treeNewick;
}
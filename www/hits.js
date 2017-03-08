
function list_hits_phylosort(results, newick, label_to_id)
{
    var t = new Tree();
	t.Parse(newick);
	
	var html = '';	
	html += '<table>';
	
	var ni = new NodeIterator(t.root);
	var q = ni.Begin();
	while (q != null)
	{
		if (q.IsLeaf())
		{
			var label = q.label;
			
			var i = label_to_id[label];
			
			html += '<tr>';
	
			html += '<td>' + i + '</td>';
			html += '<td>' + results.hits.hits[i]._source.materialSampleID + '</td>';
			html += '<td>' + results.hits.hits[i]._score + '</td>';
	
			html += '<td>';
			if (results.hits.hits[i]._source.taxonID) {
				html += results.hits.hits[i]._source.taxonID;
			}	
			html += '</td>';
	
	
			html += '<td>';
			if (results.hits.hits[i]._source.scientificName) {
				html += results.hits.hits[i]._source.scientificName;
			}	
			html += '</td>';
	
			html += '</tr>';
					
			
		}
		q = ni.Next();
	}	

	html += '</table>';
	$('#table').html(html);

}

function list_hits(results, label_to_id)
{
	//alert('x');
	// can we have an ordered list of "hits" in the case where this is like a BLAST search?
	var n = results.hits.hits.length;
	var html = '';
	html += '<table>';
	for (var i = 0; i < n; i++) {
		html += '<tr>';
	
		html += '<td>' + i + '</td>';
		html += '<td>' + results.hits.hits[i]._source.materialSampleID + '</td>';
		html += '<td>' + results.hits.hits[i]._score + '</td>';
	
		html += '<td>';
		if (results.hits.hits[i]._source.taxonID) {
			html += results.hits.hits[i]._source.taxonID;
		}	
		html += '</td>';
	
	
		html += '<td>';
		if (results.hits.hits[i]._source.scientificName) {
			html += results.hits.hits[i]._source.scientificName;
		}	
		html += '</td>';
	
		html += '</tr>';

	}
	html += '</table>';
	$('#table').html(html);
}

function hits_on_map(results, map)
{
var geojson = {};

geojson.type = "FeatureCollection";
geojson.features = [];

var n = results.hits.hits.length;
for (var i = 0; i < n; i++) {
	if (results.hits.hits[i]._source.geometry) {
	var feature = {};
	feature.type = "Feature";
	feature.geometry = results.hits.hits[i]._source.geometry;
	
	feature.properties = {};
	
	// Content for the popup							
	feature.properties.name = results.hits.hits[i]._source.materialSampleID;
	
	feature.properties.popupContent = '';
	if (results.hits.hits[i]._source.scientificName) {
		feature.properties.popupContent = results.hits.hits[i]._source.scientificName + '<br />';
	}
	
	if (results.hits.hits[i]._source.materialSampleID) {
		feature.properties.popupContent += results.hits.hits[i]._source.materialSampleID + '<br />';
	}
	if (results.hits.hits[i]._source.otherCatalogNumbers) {
		feature.properties.popupContent += results.hits.hits[i]._source.otherCatalogNumbers + '<br />';
	}
	if (results.hits.hits[i]._source.fieldNumber) {
		feature.properties.popupContent += results.hits.hits[i]._source.fieldNumber + '<br />';
	}
	if (results.hits.hits[i]._source.geneticAccessionNumber) {
		feature.properties.popupContent += '<b>' + results.hits.hits[i]._source.geneticAccessionNumber + '</b><br />';
	}
	
	
	
								
	geojson.features.push(feature);	
	}						
}
add_data(geojson);
}

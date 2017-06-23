<?php

// Import DwCA with sequences and send to Elasticsearch

require_once(dirname(dirname(__FILE__)) . '/lib.php');
require_once(dirname(dirname(__FILE__)) . '/elastic.php');

require_once(dirname(__FILE__) . '/dwca.php');

//--------------------------------------------------------------------------------------------------
function data_store_elastic($data)
{
	global $elastic;
	
	if (isset($data->_id))
	{
		switch ($data->_type)
		{
			// Occurrences and Material samples
			case "MaterialSample":
			case "Occurrence":	
			
				$id = $data->_id;
			
				$obj = new stdclass;	
				
				// For now just accept all fields
				$obj = $data;
				
				// clean up data fields we don't need and/or clash with Elasticsearch
				unset($obj->_id);
				unset($obj->_type);
				
				// Do any post processing here
				
				// Extensions
				$obj->extensions = new stdclass;
				
				// GeoJSON
				if (isset($data->decimalLatitude) && isset($data->decimalLongitude))
				{
					$obj->geometry = new stdclass;
					$obj->geometry->type = "Point";
					$obj->geometry->coordinates = array(
						(float) $data->decimalLongitude,
						(float) $data->decimalLatitude
						);			
				}

				if (0)
				{
					print_r($obj);
				}
				
				if (1) 
				{
					$doc = new stdclass;
					$doc->doc = $obj;
					$doc->doc_as_upsert = true;
					
					print_r($doc);
				
					$elastic->send('POST', 'sequence/' . urlencode($id) . '/_update', json_encode($doc));				
				}
				break;
				
			default:
				break;
		}
	}
	else
	{
		// extension, so add to existing data object
		switch ($data->_type)
		{
			case 'Amplification':
				$id = $data->_coreid;

				$obj = new stdclass;	
				
				
				// clean up data fields we don't need and/or clash with Elasticsearch
				unset($data->_coreid);
				unset($data->coreid);
				unset($data->_type);
				
				$obj = new stdclass;
				$obj->extensions['Amplification'] = array();
				$obj->extensions['Amplification'][] = $data;
				
				
				if (1)
				{
					print_r($obj);
				}
				
				if (1) 
				{
					$doc = new stdclass;	
					$doc->doc = $obj;
					$doc->doc_as_upsert = true;
					
					print_r($doc);
				
					$elastic->send('POST', 'sequence/' . urlencode($id) . '/_update', json_encode($doc));				
				}
				break;
				
			default:
				break;
		}
	}
}


//--------------------------------------------------------------------------------------------------

// Archive to parse

$archive = "../converters/dwca.zip";

$basedir = dirname(__FILE__) . '/tmp/';

$zip = new ZipArchive;
if ($zip->open($archive) === TRUE) {
    $zip->extractTo($basedir);
    $zip->close();
}

$basedir .= 'dwca/';

// meta.xml tells us how to interpret archive
$filename = $basedir . 'meta.xml';
$xml = file_get_contents($filename);

// Read details of source file(s) and extract data
$dom= new DOMDocument;
$dom->loadXML($xml);
$xpath = new DOMXPath($dom);
$xpath->registerNamespace('dwc_text', 'http://rs.tdwg.org/dwc/text/');

//parse_eml($xpath);

// set a custom function to determine how we post-process the data
parse_meta($xpath, '//dwc_text:core', 'data_store_elastic');
parse_meta($xpath, '//dwc_text:extension', 'data_store_elastic');

//print_r($geometry);

//echo json_encode($geometry);

?>

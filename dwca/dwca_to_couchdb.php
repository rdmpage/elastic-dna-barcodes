<?php

// Import DwCA with sequences and send to CouchDB

require_once(dirname(dirname(__FILE__)) . '/lib.php');
require_once(dirname(dirname(__FILE__)) . '/couchsimple.php');

require_once(dirname(__FILE__) . '/dwca.php');

//--------------------------------------------------------------------------------------------------
function data_store_couchdb($data)
{
	global $config;
	global $couch;	
	
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

				if (1)
				{
					print_r($obj);
				}
				
				$doc = new stdclass;
				$doc->_id = $id;
				$doc->doc = $obj;
				$couch->add_update_or_delete_document($doc,  $doc->_id);		
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
				
				// For now just accept all fields
				$obj = $data;
				
				// clean up data fields we don't need and/or clash with Elasticsearch
				unset($obj->_coreid);
				unset($obj->coreid);
				unset($obj->_type);
				
				if (1)
				{
					print_r($obj);
				}
				
				// add to CouchDB record
				$resp = $couch->send("GET", "/" . $config['couchdb_options']['database'] . "/" . urlencode($id));
				$doc = json_decode($resp);
				
				$doc->doc->amplification = $obj;
				
				$resp = $couch->send("PUT", "/" . $config['couchdb_options']['database'] . "/" . urlencode($id), json_encode($doc));
				var_dump($resp);				
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


// set a custom function to determine how we post-process the data
parse_meta($xpath, '//dwc_text:core', 'data_store_couchdb');
parse_meta($xpath, '//dwc_text:extension', 'data_store_couchdb');


?>

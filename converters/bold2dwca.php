<?php

require_once(dirname(dirname(__FILE__)) . '/lib.php');


//----------------------------------------------------------------------------------------
$key_to_dwca = array(

// sample
'processid' 			=> 'dwc:materialSampleID',
'sampleid'				=> 'dwc:otherCatalogNumbers',
'catalognum'			=> 'dwc:catalogNumber',
'institution_storing'	=> 'dwc:institutionCode',
'fieldnum'				=> 'dwc:fieldNumber',

// identification
'bin_uri'						=> 'dwc:taxonID',
'phylum_name'					=> 'dwc:phylum',
'class_name'					=> 'dwc:class',
'order_name'					=> 'dwc:order',
'family_name'					=> 'dwc:family',
'genus_name'					=> 'dwc:genus',
'species_name'					=> 'dwc:scientificName',
'identification_provided_by'	=> 'dwc:identifiedBy',

// event
'collectors' 		=> 'dwc:recordedBy',
'collectiondate'	=> 'dwc:eventDate',
'lifestage' 		=> 'dwc:lifestage',
'sex' 				=> 'dwc:sex',
'notes' 			=> 'dwc:occurrenceRemarks',
 
// locality
'lat'					=> 'dwc:decimalLatitude',
'lon'					=> 'dwc:decimalLongitude',
'coord_source'			=> 'dwc:georeferenceSources',
'coord_accuracy'		=> 'dwc:coordinatePrecision',
'country'				=> 'dwc:country',
'province'				=> 'dwc:stateProvince',
'exactsite'				=> 'dwc:locality',

// sequence
'genbank_accession'		=> 'dwc:associatedSequences'

);

//----------------------------------------------------------------------------------------
$key_to_ggbn = array(
'processid' 			=> 'dwc:materialSampleID',
'genbank_accession'		=> 'ggbn:geneticAccessionNumber',
'markercode'			=> 'ggbn:marker',
'nucleotides'			=> 'ggbn:consensusSequence'
);

//----------------------------------------------------------------------------------------
// Generate meta.xml file for this data
function write_meta($filename = 'meta.xml')
{
	global $key_to_dwca;
	global $key_to_ggbn;
	
	$meta = new DomDocument('1.0', 'UTF-8');
	$meta->formatOutput = true;

	$archive = $meta->createElement('archive');
	$archive->setAttribute('xmlns', 'http://rs.tdwg.org/dwc/text/');
	//$archive->setAttribute('metadata', 'eml.xml');
	$archive = $meta->appendChild($archive);


	// core
	$core = $meta->createElement('core');
	$core->setAttribute('encoding', 'utf-8');
	$core->setAttribute('fieldsTerminatedBy', '\t');
	$core->setAttribute('linesTerminatedBy', '\n');
	$core->setAttribute('fieldsEnclosedBy', '');
	$core->setAttribute('ignoreHeaderLines', '1');
	$core->setAttribute('rowType', 'http://rs.tdwg.org/dwc/terms/MaterialSample');
	$core = $archive->appendChild($core);

	// files
	$files = $core->appendChild($meta->createElement('files'));
	$location = $files->appendChild($meta->createElement('location'));
	$location->appendChild($meta->createTextNode('occurrences.tsv'));
	
	$id = $core->appendChild($meta->createElement('id'));
	$id->setAttribute('index', '0');
	
	$count = 0;
	foreach ($key_to_dwca as $k => $term)
	{
		$field = $core->appendChild($meta->createElement('field'));
		$field->setAttribute('index', $count);

		// namespaces
		$term = str_replace('dwc:', 'http://rs.tdwg.org/dwc/terms/', $term);
		
		$field->setAttribute('term', $term);
		
		$count++;
	}	
	
	// defaults
	$field = $core->appendChild($meta->createElement('field'));
	$field->setAttribute('term', 'http://rs.tdwg.org/dwc/terms/basisOfRecord');
	//$field->setAttribute('default', 'http://rs.tdwg.org/dwc/terms/MaterialSample');
	$field->setAttribute('default', 'MaterialSample');

	
	// extension
	$extension = $meta->createElement('extension');
	$extension->setAttribute('encoding', 'utf-8');
	$extension->setAttribute('fieldsTerminatedBy', '\t');
	$extension->setAttribute('linesTerminatedBy', '\n');
	$extension->setAttribute('fieldsEnclosedBy', '');
	$extension->setAttribute('ignoreHeaderLines', '1');
	$extension->setAttribute('rowType', 'http://data.ggbn.org/schemas/ggbn/terms/Amplification');
	$extension = $archive->appendChild($extension);

	// files
	$files = $extension->appendChild($meta->createElement('files'));
	$location = $files->appendChild($meta->createElement('location'));
	$location->appendChild($meta->createTextNode('sequences.tsv'));
	
	$coreid = $extension->appendChild($meta->createElement('coreid'));
	$coreid->setAttribute('index', '0');
	
	$count = 0;
	foreach ($key_to_ggbn as $k => $term)
	{

		// namespaces
		if (preg_match('/^ggbn:/', $term))
		{
			$field = $extension->appendChild($meta->createElement('field'));
			$field->setAttribute('index', $count);
		
			$term = str_replace('ggbn:', 'http://data.ggbn.org/schemas/ggbn/terms/', $term);
			$field->setAttribute('term', $term);
		}
		
		$count++;
	}		

	//echo $meta->saveXML();
	file_put_contents($filename, $meta->saveXML());
}


// project 
if (1)
{
	$url = 'http://www.boldsystems.org/index.php/API_Public/combined';
	
	// GBAP GenBank amphibians
	// FFMBH Spatial heterogeneity in the Mediterranean Biodiversity Hotspot affects barcoding accuracy of its freshwater fishes
	// INLE Barcoding of fish species from Inle Lake basin in Myanmar [INLE] see also https://dx.doi.org/10.3897%2FBDJ.4.e10539
	// DSCHA see http://dx.doi.org/10.1371/journal.pone.0099546

	$parameters = array(
		'container' => 'DSCHA',
		'format' => 'tsv'
		);
}
	
// taxon 
if (0)
{
	$url = 'http://www.boldsystems.org/index.php/API_Public/combined';

	$parameters = array(
		'taxon' => 'Agnotecous',
		//'taxon' => 'Limnonectes',
//		'taxon' => 'Xenopus',
//		'taxon' => 'Biomphalaria',
		'marker' => 'COI-5P',
		'format' => 'tsv'
		);	
}

if (0)
{
	$url = 'http://www.boldsystems.org/index.php/API_Public/combined';

	$parameters = array(
		'geo' => 'New Caledonia',
		'marker' => 'COI-5P',
		'format' => 'tsv'
		);	
}
	
$url .= '?' . http_build_query($parameters);

//echo $url . "\n";


write_meta();

$zip = new ZipArchive();
$filename = "dwca.zip";

if ($zip->open($filename, ZipArchive::CREATE)!==TRUE) {
    exit("cannot open <$filename>\n");
}

$zip->addEmptyDir('dwca');
$zip->addFile(dirname(__FILE__) . '/meta.xml', 'dwca/meta.xml');


$data = get($url);

if ($data)
{
	$lines = explode("\n", $data);

	$keys = array();
	$row_count = 0;
	
	foreach ($lines as $line)
	{
		if ($line == '') break;
		
		echo $line . "\n";
		//exit();
		
		$row = explode("\t", $line);
		
		if ($row_count == 0)
		{
			$keys = $row;
			
			// export column headings
			//echo join("\t", array_keys($key_to_dwca));
			//echo join("\t", $key_to_dwca);
			//echo "\n";
			
			file_put_contents('occurrences.tsv', join("\t", $key_to_dwca) . "\n");
			file_put_contents('sequences.tsv', join("\t", $key_to_ggbn) . "\n");
			
			
		}
		else
		{			
			$occurrence = new stdclass;
			$sequence = new stdclass;
			
			$n = count($row);
			for ($i = 0; $i < $n; $i++)
			{
				if (trim($row[$i]) != '')
				{
					// Darwin Core terms
					if (isset($key_to_dwca[$keys[$i]]))
					{
						$occurrence->{$key_to_dwca[$keys[$i]]} = $row[$i];
					}
					
					// GGBN
					if (isset($key_to_ggbn[$keys[$i]]))
					{
						$sequence->{$key_to_ggbn[$keys[$i]]} = $row[$i];
					}
				}
			}
			
			// dump as row of data for Darwin Core...
			// or can we just dump TSV file and use meta.xml cleverly?
			// having images, traces, etc. probably means we need to dump several rows of different types

			$values = array();
			foreach ($key_to_dwca as $k => $v)
			{
				if (isset($occurrence->{$v}))
				{
					$values[] = $occurrence->{$v};
				}
				else
				{
					$values[] = '';
				}
			}
			file_put_contents('occurrences.tsv', join("\t", $values) . "\n", FILE_APPEND);			

			$values = array();
			foreach ($key_to_ggbn as $k => $v)
			{
				if (isset($sequence->{$v}))
				{
					$values[] = $sequence->{$v};
				}
				else
				{
					$values[] = '';
				}
			}
			file_put_contents('sequences.tsv', join("\t", $values) . "\n", FILE_APPEND);			
			
		}
		$row_count++;
	}
}

$zip->addFile(dirname(__FILE__) . '/occurrences.tsv', 'dwca/occurrences.tsv');
$zip->addFile(dirname(__FILE__) . '/sequences.tsv', 'dwca/sequences.tsv');
$zip->close();


?>

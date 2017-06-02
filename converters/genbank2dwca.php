<?php

// Fetch some GenBank sequences and create Darwin Core Archive


require_once(dirname(dirname(__FILE__)) . '/lib.php');

require_once(dirname(__FILE__) . '/seq.php');


//----------------------------------------------------------------------------------------
$key_to_dwca = array(

// sample
'materialSampleID' 		=> 'dwc:materialSampleID', // dummy
'institutionCode'		=> 'dwc:institutionCode',
'collectionCode'		=> 'dwc:collectionCode',
'catalogNumber'			=> 'dwc:catalogNumber',
'otherCatalogNumbers'	=> 'dwc:otherCatalogNumbers',

// identification
'taxonID'						=> 'dwc:taxonID',
'higherClassification'			=> 'dwc:higherClassification',
'organism'						=> 'dwc:scientificName',
//'identification_provided_by'	=> 'dwc:identifiedBy',

// event
'collected_by' 			=> 'dwc:recordedBy',
'eventDate'				=> 'dwc:eventDate',
'collection_date'		=> 'dwc:verbatimEventDate',
'note' 					=> 'dwc:occurrenceRemarks',
 
// locality
'lat_lon'				=> 'dwc:verbatimCoordinates',
'decimalLatitude'		=> 'dwc:decimalLatitude',
'decimalLongitude'		=> 'dwc:decimalLongitude',
'country'				=> 'dwc:country',
'locality'				=> 'dwc:locality',

// sequence
'GBSeq_locus'			=> 'dwc:associatedSequences'

);

//----------------------------------------------------------------------------------------
// Generate meta.xml file for this data
function write_meta($filename = 'meta.xml')
{
	global $key_to_dwca;
	
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

	
	// sequences--------------------------------------------------------------------------
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
	
	$field = $extension->appendChild($meta->createElement('field'));
	$field->setAttribute('index', '1');
	$field->setAttribute('term', 'http://data.ggbn.org/schemas/ggbn/terms/geneticAccessionNumber');
	
	$field = $extension->appendChild($meta->createElement('field'));
	$field->setAttribute('index', '2');
	$field->setAttribute('term', 'http://data.ggbn.org/schemas/ggbn/terms/consensusSequence');
  	
	// references-------------------------------------------------------------------------
	$extension = $meta->createElement('extension');
	$extension->setAttribute('encoding', 'utf-8');
	$extension->setAttribute('fieldsTerminatedBy', '\t');
	$extension->setAttribute('linesTerminatedBy', '\n');
	$extension->setAttribute('fieldsEnclosedBy', '');
	$extension->setAttribute('ignoreHeaderLines', '1');
	$extension->setAttribute('rowType', 'http://rs.gbif.org/terms/1.0/Reference');
	$extension = $archive->appendChild($extension);

	// files
	$files = $extension->appendChild($meta->createElement('files'));
	$location = $files->appendChild($meta->createElement('location'));
	$location->appendChild($meta->createTextNode('references.tsv'));
	
	$coreid = $extension->appendChild($meta->createElement('coreid'));
	$coreid->setAttribute('index', '0');
	
	$field = $extension->appendChild($meta->createElement('field'));
	$field->setAttribute('index', '1');
	$field->setAttribute('term', 'http://purl.org/dc/terms/identifier');
			
	$field = $extension->appendChild($meta->createElement('field'));
	$field->setAttribute('index', '2');
	$field->setAttribute('term', 'http://purl.org/dc/terms/bibliographicCitation');

	//echo $meta->saveXML();
	file_put_contents($filename, $meta->saveXML());
}



write_meta();


$zip = new ZipArchive();
$filename = "dwca.zip";

if ($zip->open($filename, ZipArchive::CREATE)!==TRUE) {
    exit("cannot open <$filename>\n");
}

$zip->addEmptyDir('dwca');
$zip->addFile(dirname(__FILE__) . '/meta.xml', 'dwca/meta.xml');


// get some data
$ids=array('KP420745');

$ids=array(
'KC860806',
'KC860814',
'KC860810',
'KC860808',
'KC860807',
'KC860804',
'KC860803',
'KF929841',
'KC860818',
'KC860824',
'KC860812',
'GU224789',
'JQ840831',
'KC860816',
'JQ840834',
'KC860820',
'KC860819',
'KC860813',
'KC860811',
'KC860809',
'KC860805',
'GU224788',
'HQ168534',
'KC860849',
'KC860850',
'JQ842099',
'JQ842105',
'JQ840490',
'JQ842104',
'JQ841556',
'JQ842103',
'JQ842101',
'KC860828',
'KC860831',
'KC860830',
'KC860792',
'JQ841562',
'KC860793',
'KC860836',
'KC860829'
);

$ids=array(
'JF312106',
'JF312149',
'JF312090',
'JF312109',
'JF312152',
'JF312186',
'JQ173889',
'KP336714',
'KJ699124',
'AB842501',
'JF792341',
'GQ481252',
'JN801706',
'JQ175004',
'GU571337',
'GQ922615',
'JF792347',
'JF792344',
'JF312179',
'JF312175',
'JF312169',
'JF312163',
'JF312162',
'JF312086',
'JF312099',
'JF312098',
'JF312093',
'JF312095',
'JQ175005',
'JF312182',
'JF312158',
'JF312094',
'JQ173892',
'EF515767',
'AB843326',
'GU571822',
'JF312213',
'JF312207',
'JF312116',
'JF312092'
);

$ids=array(
'HQ619940',
'HQ619944',
'HQ619943',
'HQ619941',
'HQ619951',
'HQ619945',
'HQ619947',
'HQ619946',
'HQ619948',
'HQ619942',
'HQ619949',
'HQ619950',
'HQ619952',
'FJ383132',
'FJ383133',
'KJ095174',
'EU091257',
'KJ095177',
'KJ095176',
'FJ383130',
'KJ095178',
'HQ619936',
'KJ095175',
'FJ383129',
'EU091258',
'HQ619934',
'HQ619935',
'KJ095179',
'EU091255',
'KJ095181',
'JF440281',
'KJ095193',
'KJ095192',
'KJ095191',
'KJ095182',
'KJ095180',
'KJ095184',
'KJ095189',
'JF440280',
'KJ095190'
);


$data = fetch_sequences($ids);

//print_r($data);

if ($data)
{
	// header rows
	
	// occurrences
	//echo join("\t", array_keys($key_to_dwca)) . "\n";
	file_put_contents('occurrences.tsv', join("\t", $key_to_dwca) . "\n");

	// sequences
	//echo "dwc:materialSampleID\tggbn:geneticAccessionNumber\tggbn:consensusSequence\n";
	file_put_contents('sequences.tsv', "dwc:materialSampleID\tggbn:geneticAccessionNumber\tggbn:consensusSequence\n");

	// references
	//echo "dwc:materialSampleID\tdcterms:identifier\tdcterms:bibliographicCitation\n";
	file_put_contents('references.tsv', "dwc:materialSampleID\tdcterms:identifier\tdcterms:bibliographicCitation\n");
	
	// output data
	foreach ($data as $seq)
	{
		// occurrence
		$row = array();
				
		foreach ($key_to_dwca as $k => $v)
		{
			$value = '';
		
			if (isset($seq->{$k}))
			{
				switch ($k)
				{						
					case 'eventDate':
						$value = join('-', $seq->{$k});
						break;
			
					case 'higherClassification':
						$value = str_replace('; ', ' | ', $seq->{$k});
						break;
					
					case 'otherCatalogNumbers':
						$value = $seq->{$k}[0];
						break;
					
					default:
						$value = $seq->{$k};
						break;
				}
			}
		
			$row[] = $value;
		}
		
		//print_r(array_keys($key_to_dwca));
		//print_r($row);
		
		// output
		//echo join("\t", $row) . "\n";
		file_put_contents('occurrences.tsv', join("\t", $row) . "\n", FILE_APPEND);
		
	
		// sequence
		if (isset($seq->GBSeq_sequence))
		{
			// dwc:materialSampleID	ggbn:geneticAccessionNumber	ggbn:marker	ggbn:consensusSequence
			
			$row = array();
			
			// coreid
			$row[] = $seq->GBSeq_locus;
			
			// accession
			$row[] = $seq->GBSeq_locus;
			
			// sequence
			$row[] = $seq->GBSeq_sequence;
			
			// output
			// echo join("\t", $row) . "\n";
			file_put_contents('sequences.tsv', join("\t", $row) . "\n", FILE_APPEND);
		}
		
	
		// references
		if (isset($seq->reference))
		{
			foreach ($seq->reference as $reference)
			{
				$row = array();
				
				$row[] = $seq->GBSeq_locus;
			
				$identifier = '';
				if ($identifier == '')
				{
					if (isset($reference->doi))
					{
						$identifier = 'doi:' . $reference->doi;
					}
				}
				if ($identifier == '')
				{
					if (isset($reference->pmid))
					{
						$identifier = 'pmid:' . $reference->pmid;
					}
				}
				$row[] = $identifier;
				
				$bibliographicCitation= '';
				
				if (isset($reference->author))
				{
					$bibliographicCitation = join("; ", $reference->author);
				}
				if (isset($reference->title))
				{
					$bibliographicCitation .= ' ' . $reference->title . '.';
				}
				if (isset($reference->journal))
				{
					$bibliographicCitation .= ' ' . $reference->journal;
				}
				
				$row[] = trim($bibliographicCitation);
				 				
				// output
				//echo join("\t", $row) . "\n";
				file_put_contents('references.tsv', join("\t", $row) . "\n", FILE_APPEND);
			}
		}
	}


}


$zip->addFile(dirname(__FILE__) . '/occurrences.tsv', 'dwca/occurrences.tsv');
$zip->addFile(dirname(__FILE__) . '/references.tsv', 'dwca/references.tsv');
$zip->addFile(dirname(__FILE__) . '/sequences.tsv', 'dwca/sequences.tsv');
$zip->close();



?>

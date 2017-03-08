<?php

// sequences from ids

error_reporting(E_ALL);

require_once (dirname(dirname(__FILE__)) . '/lib.php');

//----------------------------------------------------------------------------------------
// Convert NCBI style date (e.g., "07-OCT-2015") to array
function parse_ncbi_date($date_string)
{
	$date_array = array();
	
	if (false != strtotime($date_string))
	{
		// format without leading zeros
		$ymd = date("Y-m-d", strtotime($date_string));
		
		$date_array = explode('-', $ymd);		
	}	
	
	return $date_array;
}

//----------------------------------------------------------------------------------------
/**
 * @brief Convert degrees, minutes, seconds to a decimal value
 *
 * @param degrees Degrees
 * @param minutes Minutes
 * @param seconds Seconds
 * @param hemisphere Hemisphere (optional)
 *
 * @result Decimal coordinates
 */
function degrees2decimal($degrees, $minutes=0, $seconds=0, $hemisphere='N')
{
	$result = $degrees;
	$result += $minutes/60.0;
	$result += $seconds/3600.0;
	
	if ($hemisphere == 'S')
	{
		$result *= -1.0;
	}
	if ($hemisphere == 'W')
	{
		$result *= -1.0;
	}
	// Spanish
	if ($hemisphere == 'O')
	{
		$result *= -1.0;
	}
	// Spainish OCR error
	if ($hemisphere == '0')
	{
		$result *= -1.0;
	}
	
	return $result;
}

//----------------------------------------------------------------------------------------
function process_lat_lon(&$genbank_record, $lat_lon)
{

	$matched = false;

	if (preg_match ("/(N|S)[;|,] /", $lat_lon))
	{
		// it's a literal string description, not a pair of decimal coordinates.
		if (!$matched)
		{
			//  35deg12'07'' N; 83deg05'2'' W, e.g. DQ995039
			if (preg_match("/([0-9]{1,2})deg([0-9]{1,2})'(([0-9]{1,2})'')?\s*([S|N])[;|,]\s*([0-9]{1,3})deg([0-9]{1,2})'(([0-9]{1,2})'')?\s*([W|E])/", $lat_lon, $matches))
			{
				//print_r($matches);
			
				$degrees = $matches[1];
				$minutes = $matches[2];
				$seconds = $matches[4];
				$hemisphere = $matches[5];
				$lat = $degrees + ($minutes/60.0) + ($seconds/3600);
				if ($hemisphere == 'S') { $lat *= -1.0; };

				$genbank_record->decimalLatitude = $lat;

				$degrees = $matches[6];
				$minutes = $matches[7];
				$seconds = $matches[9];
				$hemisphere = $matches[10];
				$long = $degrees + ($minutes/60.0) + ($seconds/3600);
				if ($hemisphere == 'W') { $long *= -1.0; };
				
				$genbank_record->decimalLongitude = $long;
				
				$matched = true;
			}
		}
		if (!$matched)
		{
			
			list ($lat, $long) = explode ("; ", $lat_lon);

			list ($degrees, $rest) = explode (" ", $lat);
			list ($minutes, $rest) = explode ('.', $rest);

			list ($decimal_minutes, $hemisphere) = explode ("'", $rest);

			$lat = $degrees + ($minutes/60.0) + ($decimal_minutes/6000);
			if ($hemisphere == 'S') { $lat *= -1.0; };

			$genbank_record->decimalLatitude = $lat;

			list ($degrees, $rest) = explode (" ", $long);
			list ($minutes, $rest) = explode ('.', $rest);

			list ($decimal_minutes, $hemisphere) = explode ("'", $rest);

			$long = $degrees + ($minutes/60.0) + ($decimal_minutes/6000);
			if ($hemisphere == 'W') { $long *= -1.0; };
			
			$genbank_record->decimalLongitude = $long;
			
			$matched = true;
		}

	}
	
	if (!$matched)
	{			
		// N19.49048, W155.91167 [EF219364]
		if (preg_match ("/(?<lat_hemisphere>(N|S))(?<latitude>(\d+(\.\d+))), (?<long_hemisphere>(W|E))(?<longitude>(\d+(\.\d+)))/", $lat_lon, $matches))
		{
			$lat = $matches['latitude'];
			if ($matches['lat_hemisphere'] == 'S') { $lat *= -1.0; };
			
			$genbank_record->decimalLatitude = $lat;
			
			$long = $matches['longitude'];
			if ($matches['long_hemisphere'] == 'W') { $long *= -1.0; };
			
			$genbank_record->decimalLongitude = $long;
			
			$matched = true;

		}
	}
	
	if (!$matched)		
	{
		//13.2633 S 49.6033 E
		if (preg_match("/([0-9]+(\.[0-9]+)*) ([S|N]) ([0-9]+(\.[0-9]+)*) ([W|E])/", $lat_lon, $matches))
		{
			//print_r ($matches);
			
			$lat = $matches[1];
			if ($matches[3] == 'S') { $lat *= -1.0; };
			
			$genbank_record->decimalLatitude = $lat;

			$long = $matches[4];
			if ($matches[6] == 'W') { $long *= -1.0; };
			
			$genbank_record->decimalLongitude = $long;
			
			$matched = true;
		}
	}
	
	
	// AY249471 Palmer Archipelago 64deg51.0'S, 63deg34.0'W 
	if (!$matched)		
	{
		if (preg_match("/(?<lat_deg>[0-9]{1,2})deg(?<lat_min>[0-9]{1,2}(\.\d+)?)'\s*(?<lat_hemisphere>[S|N]),?\s*(?<long_deg>[0-9]{1,3})deg(?<long_min>[0-9]{1,2}(\.\d+)?)'\s*(?<long_hemisphere>[W|E])/", $lat_lon, $matches))
		{
			//print_r ($matches);
			
			$genbank_record->decimalLatitude
				= degrees2decimal(
					$matches['lat_deg'], 
					$matches['lat_min'], 
					0,
					$matches['lat_hemisphere']
					);

			$genbank_record->decimalLongitude
				= degrees2decimal(
					$matches['long_deg'], 
					$matches['long_min'], 
					0,
					$matches['long_hemisphere']
					);
			
			/*
			//exit();
			
			$lat = $matches[1];
			if ($matches[3] == 'S') { $lat *= -1.0; };
			$sequence->source->latitude = $lat;

			$long = $matches[4];
			if ($matches[6] == 'W') { $long *= -1.0; };
			
			$sequence->source->longitude = $long;
			*/
			
			//print_r($sequence);
			//exit();
			
			$matched = true;
		}
	}
	
	if (!$matched)
	{
		
		if (preg_match("/(?<latitude>\-?\d+(\.\d+)?),?\s*(?<longitude>\-?\d+(\.\d+)?)/", $lat_lon, $matches))
		{
			//print_r($matches);
			
			$genbank_record->decimalLatitude  = $matches['latitude'];
			$genbank_record->decimalLongitude = $matches['longitude'];
		
			$matched = true;
		}
	}
}

//----------------------------------------------------------------------------------------
function process_locality(&$genbank_record)
{
	$debug = false;
		
	if (isset($genbank_record->country))
	{
		$country = $genbank_record->country;

		$matches = array();	
		$parts = explode (":", $country);	
		$genbank_record->country = $parts[0];
		
		$locality_string = '';
		
		if (count($parts) > 1)
		{
			$locality_string = trim($parts[1]);
			$genbank_record->locality = $locality_string;
			// Clean up
			$genbank_record->locality = preg_replace('/\(?GPS/', '', $locality_string);				
		

			// Handle AMNH stuff
			if (preg_match('/(?<latitude_degrees>[0-9]+)deg(?<latitude_minutes>[0-9]{1,2})\'\s*(?<latitude_hemisphere>[N|S])/i', $locality_string, $matches))
			{
				if ($debug) { print_r($matches); }	

				$degrees = $matches['latitude_degrees'];
				$minutes = $matches['latitude_minutes'];
				$hemisphere = $matches['latitude_hemisphere'];
				$lat = $degrees + ($minutes/60.0);
				if ($hemisphere == 'S') { $lat *= -1.0; };

				$genbank_record->decimalLatitude  = $lat;
			}

			if (preg_match('/(?<longitude_degrees>[0-9]+)deg(,\s*)?(?<longitude_minutes>[0-9]{1,2})\'\s*(?<longitude_hemisphere>[W|E])/i', $locality_string, $matches))
			{
		
				if ($debug) { print_r($matches); }	
			
				$degrees = $matches['longitude_degrees'];
				$minutes = $matches['longitude_minutes'];
				$hemisphere = $matches['longitude_hemisphere'];
				$long = $degrees + ($minutes/60.0);
				if ($hemisphere == 'W') { $long *= -1.0; };
			
				$genbank_record->decimalLongitude  = $long;
			}

			$matched = false;
			// AY249471 Palmer Archipelago 64deg51.0'S, 63deg34.0'W 
			if (preg_match("/(?<latitude_degrees>[0-9]{1,2})deg(?<latitude_minutes>[0-9]{1,2}(\.\d+)?)'\s*(?<latitude_hemisphere>[S|N]),\s*(?<longitude_degrees>[0-9]{1,3})deg(?<longitude_minutes>[0-9]{1,2}(\.\d+)?)'\s*(?<longitude_hemisphere>[W|E])/", $locality_string, $matches))
			{	
			
				if ($debug) { print_r($matches); }	

				$degrees = $matches['latitude_degrees'];
				$minutes = $matches['latitude_minutes'];
				$hemisphere = $matches['latitude_hemisphere'];
				$lat = $degrees + ($minutes/60.0);
				if ($hemisphere == 'S') { $lat *= -1.0; };

				$genbank_record->decimalLatitude = $lat;

				$degrees = $matches['longitude_degrees'];
				$minutes = $matches['longitude_minutes'];
				$hemisphere = $matches['longitude_hemisphere'];
				$long = $degrees + ($minutes/60.0);
				if ($hemisphere == 'W') { $long *= -1.0; };
				
				$genbank_record->decimalLongitude  = $long;
				
				$matched = true;
			}
			
			if (!$matched)
			{
				
				//26'11'24N 81'48'16W
				
				//echo $seq['source']['locality'] . "\n";
				
				if (preg_match("/
				(?<latitude_degrees>[0-9]{1,2})
				'
				(?<latitude_minutes>[0-9]{1,2})
				'
				((?<latitude_seconds>[0-9]{1,2})
				'?)?
				(?<latitude_hemisphere>[S|N])
				\s+
				(?<longitude_degrees>[0-9]{1,3})
				'
				(?<longitude_minutes>[0-9]{1,2})
				'
				((?<longtitude_seconds>[0-9]{1,2})
				'?)?
				(?<longitude_hemisphere>[W|E])
				/x", $locality_string, $matches))
				{
					if ($debug) { print_r($matches); }	
						
					$degrees = $matches['latitude_degrees'];
					$minutes = $matches['latitude_minutes'];
					$seconds = $matches['latitude_seconds'];
					$hemisphere = $matches['latitude_hemisphere'];
					$lat = $degrees + ($minutes/60.0) + ($seconds/3600);
					if ($hemisphere == 'S') { $lat *= -1.0; };
	
					$genbank_record->decimalLatitude = $lat;
	
					$degrees = $matches['longitude_degrees'];
					$minutes = $matches['longitude_minutes'];
					$seconds = $matches['longtitude_seconds'];
					$hemisphere = $matches['longitude_hemisphere'];
					$long = $degrees + ($minutes/60.0) + ($seconds/3600);
					if ($hemisphere == 'W') { $long *= -1.0; };
					
					$genbank_record->decimalLongitude = $long;
					
					//print_r($seq);
					
					//exit();
					
					$matched = true;
				}
			}
			//exit();

			
		}
		
		if ($debug)
		{
			echo "Trying line " . __LINE__ . "\n";
		}
		
		
		//(GPS: 33 38' 07'', 146 33' 12'') e.g. AY281244
		if (preg_match("/\(GPS:\s*([0-9]{1,2})\s*([0-9]{1,2})'\s*([0-9]{1,2})'',\s*([0-9]{1,3})\s*([0-9]{1,2})'\s*([0-9]{1,2})''\)/", $country, $matches))
		{
			if ($debug) { print_r($matches); }	
			
			$lat = $matches[1] + $matches[2]/60 + $matches[3]/3600;
			
			// OMG
			if ($seq['source']['country'] == 'Australia')
			{
				$lat *= -1.0;
			}
			$long = $matches[4] + $matches[5]/60 + $matches[6]/3600;

			$genbank_record->decimalLatitude  = $lat;
			$genbank_record->decimalLongitude  = $long;
			
		}
		
		if ($debug)
		{
			echo "Trying line " . __LINE__ . "\n";
		}
		
		
		// AJ556909
		// 2o54'59''N 98o38'24''E			
		if (preg_match("/
			(?<latitude_degrees>[0-9]{1,2})
			o
			(?<latitude_minutes>[0-9]{1,2})
			'
			(?<latitude_seconds>[0-9]{1,2})
			''
			(?<latitude_hemisphere>[S|N])
			\s+
			(?<longitude_degrees>[0-9]{1,3})
			o
			(?<longitude_minutes>[0-9]{1,2})
			'
			(?<longtitude_seconds>[0-9]{1,2})
			''
			(?<longitude_hemisphere>[W|E])
			/x", $locality_string, $matches))
		{
			if ($debug) { print_r($matches); }	
				
			$degrees = $matches['latitude_degrees'];
			$minutes = $matches['latitude_minutes'];
			$seconds = $matches['latitude_seconds'];
			$hemisphere = $matches['latitude_hemisphere'];
			$lat = $degrees + ($minutes/60.0) + ($seconds/3600);
			if ($hemisphere == 'S') { $lat *= -1.0; };

			$genbank_record->decimalLatitude = $lat;

			$degrees = $matches['longitude_degrees'];
			$minutes = $matches['longitude_minutes'];
			$seconds = $matches['longtitude_seconds'];
			$hemisphere = $matches['longitude_hemisphere'];
			$long = $degrees + ($minutes/60.0) + ($seconds/3600);
			if ($hemisphere == 'W') { $long *= -1.0; };
			
			$genbank_record->decimalLongitude = $long;
			
		}
		
		
	}
	
	if ($debug)
	{
		echo "Trying line " . __LINE__ . "\n";
	}
	/*
	// Some records have lat and lon in isolation_source, e.g. AY922971
	if (isset($genbank_record->isolation_source))
	{
		$isolation_source = $genbank_record->isolation_source;
		$matches = array();
		if (preg_match('/([0-9]+\.[0-9]+) (N|S), ([0-9]+\.[0-9]+) (W|E)/i', $isolation_source, $matches))
		{
			if ($debug) { print_r($matches); }	
			
			$genbank_record->{'http://rs.tdwg.org/dwc/terms/decimalLatitude'}[0] = $matches[1];
			if ($matches[2] == 'S')
			{
				$seq['source']['latitude'] *= -1;
			}
			$genbank_record->{'http://rs.tdwg.org/dwc/terms/decimalLongitude'}[0] = $matches[3];
			if ($matches[4] == 'W')
			{
				$seq['source']['longitude'] *= -1;
			}
			if  (!isset($genbank_record->decimalLocality))
			{
				$genbank_record->decimalLocality = $genbank_record->isolation_source;
			}
		}
	}
	*/
	
}	

//----------------------------------------------------------------------------------------
function fetch_sequences($ids)
{
	$hits = array();
	
	// Query URL	
	$url = 'https://eutils.ncbi.nlm.nih.gov/entrez/eutils/efetch.fcgi?db=nucleotide&id='
		. preg_replace('/\s+/', '', join(",", $ids))
		. '&rettype=gb&retmode=xml';
	
	
	//echo $url . '<br/>';
	
	$xml = get($url);
	
	echo "Get sequences\n";
	
	echo $xml;
	
	
	// process
	if ($xml != '')
	{
		$dom = new DOMDocument;
		$dom->loadXML($xml);
		$xpath = new DOMXPath($dom);
	
	
		$GBSeqs = $xpath->query ('//GBSeq');
		foreach ($GBSeqs as $GBSeq)
		{	
			$genbank_record = new stdclass;
		
		
			$accs = $xpath->query ('GBSeq_locus', $GBSeq);
			foreach ($accs as $acc)
			{
				$genbank_record->GBSeq_locus = $acc->firstChild->nodeValue;
			}
			
			// references
			$genbank_record->reference = array();
			$references = $xpath->query ('GBSeq_references/GBReference', $GBSeq);
			foreach ($references as $reference)
			{
				$ref = new stdclass;
				
				$titles = $xpath->query ('GBReference_title', $reference);
				foreach ($titles as $title)
				{
					$ref->title = $title->firstChild->nodeValue;
				}
								
				// authors
				$authors = $xpath->query ('GBReference_authors/GBAuthor', $reference);
				foreach ($authors as $author)
				{
					$ref->author[] = $author->firstChild->nodeValue;
				}
								
				$journals = $xpath->query ('GBReference_journal', $reference);
				foreach ($journals as $journal)
				{
					$ref->journal = $journal->firstChild->nodeValue;
				}
				
				// DOI
				$xrefs = $xpath->query ('GBReference_xref/GBXref', $reference);
				foreach ($xrefs as $xref)
				{
					$k = '';
					$v = '';

					$names = $xpath->query ('GBXref_dbname', $xref);
					foreach ($names as $name)
					{
						$k = $name->firstChild->nodeValue;
					}
					$values = $xpath->query ('GBXref_id', $xref);
					foreach ($values as $value)
					{
						$v = $value->firstChild->nodeValue;
					}
					
					$ref->{$k} = $v;					
				}
								
				// PMID
				$pmids = $xpath->query ('GBReference_pubmed', $reference);
				foreach ($pmids as $pmid)
				{
					$ref->pmid = $pmid->firstChild->nodeValue;
				}

				switch ($ref->title)
				{
					case 'Direct Submission':
						break;
						
					default:
						$genbank_record->reference[] = $ref;
						break;
				}
			}
			
			// features
			$quals = $xpath->query ('GBSeq_feature-table/GBFeature/GBFeature_quals/GBQualifier', $GBSeq);
			foreach ($quals as $qual)
			{
				//echo 'x';
				$k = '';
				$v = '';

				$names = $xpath->query ('GBQualifier_name', $qual);
				foreach ($names as $name)
				{
					$k = $name->firstChild->nodeValue;
				}
				$values = $xpath->query ('GBQualifier_value', $qual);
				foreach ($values as $value)
				{
					$v = $value->firstChild->nodeValue;
				}
				
				switch ($k)
				{
				
					case 'collection_date':
						$genbank_record->{$k} = $v;

						// day month year
						if (preg_match('/^[0-9]{2}-[A-Z][a-z]{2}-[0-9]{4}$/', $genbank_record->collection_date))
						{
							if (false != strtotime($genbank_record->collection_date))
							{
								$genbank_record->eventDate = parse_ncbi_date($genbank_record->collection_date);
							}
						}
						
						// month year
						if (preg_match('/^[A-Z][a-z]{2}-[0-9]{4}$/', $genbank_record->collection_date))
						{
							if (false != strtotime($genbank_record->collection_date))
							{
								$genbank_record->eventDate = explode("-", date("Y-n", strtotime($genbank_record->collection_date)));
							}
						}

						// Year only
						if (preg_match('/^[0-9]{4}$/', $genbank_record->collection_date))
						{
							$genbank_record->eventDate[] = $genbank_record->collection_date;
						}
						
						break;

					case 'country':
						$genbank_record->{$k} = $v;
						break;
						
					case 'identified_by':
						$genbank_record->{$k} = $v;
						break;
											
					case 'isolate':
						$genbank_record->{$k} = $v;
						break;						

					case 'lat_lon':
						$genbank_record->{$k} = $v;
						break;
						
					case 'note':
						$genbank_record->{$k} = $v;
						break;						
												
					case 'specimen_voucher':
						$genbank_record->{$k} = $v;
						
						$genbank_record->otherCatalogNumbers[] = $genbank_record->{$k};
						
						// Try to interpret them
						$matched = false;
						
						// TM<ZAF>40766
						if (!$matched)
						{
							if (preg_match('/^(?<institutionCode>(?<prefix>[A-Z]+)\<[A-Z]+\>)(?<catalogNumber>\d+)$/', $genbank_record->{$k}, $m))
							{
								$genbank_record->institutionCode 	=  $m['institutionCode'];
								$genbank_record->catalogNumber 	=  $m['catalogNumber'];
								$matched = true;
							}
						}
															
						if (!$matched)
						{
							if (preg_match('/^(?<institutionCode>[A-Z]+):(?<collectionCode>.*):(?<catalogNumber>([A-Z]\-)?\d+)$/', $genbank_record->{$k}, $m))
							{
								$genbank_record->institutionCode 	= $m['institutionCode'];
								$genbank_record->collectionCode 	= $m['collectionCode'];
								$genbank_record->catalogNumber 		= $m['catalogNumber'];
								
								$genbank_record->otherCatalogNumbers[] = $genbank_record->institutionCode . ' ' . $genbank_record->catalogNumber ;
								
								$matched = true;
							}
						}
						
						if (!$matched)
						{
							if (preg_match('/^(?<institutionCode>[A-Z]+)[\s|:]?(?<catalogNumber>\d+)$/', $genbank_record->{$k}, $m))
							{
								$genbank_record->institutionCode 	=  $m['institutionCode'];
								$genbank_record->catalogNumber 		=  $m['catalogNumber'];
								
								// post process to help matching
								switch ($m['institutionCode'])
								{
									case 'KUNHM':
										$genbank_record->otherCatalogNumbers[] = 'KU' .  ' ' . $m['catalogNumber'];
										break;
										
									default:
										break;
								}
								$matched = true;
							}
						}
						break;
												
					default:
						break;
				}			

			}
			
			// post-process locality
			if (isset($genbank_record->lat_lon))
			{			
				process_lat_lon($genbank_record, $genbank_record->lat_lon);
			}
			if (isset($genbank_record->country))
			{			
				process_locality($genbank_record);
			}				
			
			$seqs = $xpath->query ('GBSeq_sequence', $GBSeq);
			foreach ($seqs as $seq)
			{
				$genbank_record->GBSeq_sequence = strtoupper($seq->firstChild->nodeValue);
			}
			
			$hits[] = $genbank_record;			

		}
	}
	
	print_r($hits);

	
}

//----------------------------------------------------------------------------------------

$ids = array('JF744363.1');

//$ids=array('KP420745');
$ids=array('HM067338');
//$ids=array('FR686779');
$ids=array('GQ260888');
$ids=array('EU594660.1');

$ids=array('KT728116.1');

$hits = fetch_sequences($ids);



?>
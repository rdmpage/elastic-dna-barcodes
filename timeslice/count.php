<?php

require_once('../lib.php');


/*
BCT	Bacterial sequences
PRI	Primate sequences
ROD	Rodent sequences
MAM	Other mammalian sequences
VRT	Other vertebrate sequences
INV	Invertebrate sequences
PLN	Plant and Fungal sequences
VRL	Viral sequences
PHG	Phage sequences
RNA	Structural RNA sequences
SYN	Synthetic and chimeric sequ
UNA	Unannotated sequences
*/

$start_year = 2010;
$end_year	= 2017;

$data = array();
$urls = array();



for ($year = $start_year; $year <= $end_year; $year++)
{
/*	for ($month = 1; $month <= 2; $month++)
	{
		$last_day = cal_days_in_month(CAL_GREGORIAN, $month, $year);
		
		$mindate = $year . '/' . str_pad($month, 2, '0', STR_PAD_LEFT) . '/01';
		$maxdate = $year . '/' . str_pad($month, 2, '0', STR_PAD_LEFT) . '/' . $last_day;
*/
	
	// years
	$month = 1;
	{
		$last_day = cal_days_in_month(CAL_GREGORIAN, 12, $year);
		
		$mindate = $year . '/' . str_pad($month, 2, '0', STR_PAD_LEFT) . '/01';
		$maxdate = $year . '/12/' . $last_day;

		
		$data[$mindate] = array();

		$divs = array('PRI', 'ROD', 'MAM', 'VRT', 'INV', 'PLN');

		foreach ($divs as $div)
		{
			$data[$mindate][$div] = 0;

			// restrict search
			$term = 'ddbj embl genbank with limits[filt] NOT transcriptome[All Fields] NOT mRNA[filt] NOT TSA[All Fields] NOT scaffold[All Fields]';

			// geography
			if (1)
			{
				$term .= 'AND src lat lon[prop]'; // lat_lon field
			}
			else
			{
				$term .= 'AND src country[prop] NOT src lat lon[prop] '; // country but no lat_lon field
			}
		
			// timeslice
			$term .= 'AND ' . $mindate . ':' . $maxdate . '[pdat]';

			// what taxa?
			$term .= 'AND gbdiv_' . strtolower($div) . '[PROP] AND srcdb_genbank[PROP]';
	
	

			 //AND src lat lon[prop] AND 2016/01/01:2016/06/31[pdat] AND gbdiv_inv[PROP] AND srcdb_genbank[PROP]

			$parameters = array(
				'db' => 'nucleotide',
				'term' => $term
				);
		
			$url = 'https://eutils.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?' . http_build_query($parameters);
	
			echo $url . "\n";
			
			$xml = get($url);
			
			
			$dom= new DOMDocument;
			$dom->loadXML($xml);
			$xpath = new DOMXPath($dom);
		
			$ids = array();
		
			$nodeCollection = $xpath->query ('//eSearchResult/Count');     
			foreach($nodeCollection as $node)
			{
				$count = $node->firstChild->nodeValue;
				
				$data[$mindate][$div] = $count;
			}		
			
			//print_r($data);
			
			/*
			// pause between
			{
				$rand = 1000000;
				usleep($rand);
			}	
			*/
			
			
		}
	}
}

print_r($data);

// dump

echo "Date\t" . join("\t", $divs) . "\n";

foreach ($data as $date => $counts)
{
	echo "$date\t" . join("\t", $counts) . "\n";
	

}

?>
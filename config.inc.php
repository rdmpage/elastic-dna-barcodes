<?php

// $Id: //

/**
 * @file config.php
 *
 * Global configuration variables (may be added to by other modules).
 *
 */

global $config;

// Date timezone
date_default_timezone_set('UTC');


// Proxy settings for connecting to the web-----------------------------------------------
// Set these if you access the web through a proxy server. 
$config['proxy_name'] 	= '';
$config['proxy_port'] 	= '';

//$config['proxy_name'] 	= 'wwwcache.gla.ac.uk';
//$config['proxy_port'] 	= '8080';


// Elastic--------------------------------------------------------------------------------

// Bitnami https://google.bitnami.com/vms/bitnami-elasticsearch-dm-4693
$config['elastic_options'] = array(
		'index' => 'elasticsearch/dna',
		'protocol' => 'http',
		'host' => '130.211.107.26',
		'port' => 80
		);
		
		
// Local Docker Elasticsearch version 5.6.4 http://localhost:32769
$config['elastic_options'] = array(
		'index' => 'dna',
		'protocol' => 'http',
		'host' => '127.0.0.1',
		'port' => 32769
		);
		
		
// CouchDB--------------------------------------------------------------------------------
$config['couchdb_options'] = array(
	'database' => 'elastic',
	'host' => 'localhost',
	'port' => 5984,
	'prefix' => 'http://'
	);		


$config['stale'] = true;

	
?>
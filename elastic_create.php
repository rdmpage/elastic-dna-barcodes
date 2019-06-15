<?php

// Create Elastic Search indexes

require_once(dirname(__FILE__) . '/lib.php');
require_once(dirname(__FILE__) . '/elastic.php');


// Index for DWCA JSON
$index_json = '{
	"settings" : {
		"analysis" : {
			"analyzer" : {
				"my_ngram_analyzer" : {
					"tokenizer" : "my_ngram_tokenizer"
				}
			},
			"tokenizer" : {
				"my_ngram_tokenizer" : {
					"type" : "nGram",
					"min_gram" : "5",
					"max_gram" : "5",
					"token_chars": [ "letter", "digit" ]
				}
			}
		}
	},
   "mappings": {
	 "sequence": {
		"properties": {
           "consensusSequence": {
			  "type": "string",
              "analyzer":"my_ngram_analyzer"
			},
			"geometry": {
			  "type": "geo_shape"	
			}
		}
	  }
   }
}';

// Create index
$elastic->send('PUT', '', $index_json);	


?>
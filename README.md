# elastic-dna-barcodes
Using Elasticsearch to analyse DNA barcodes and GenBank sequences, with thoughts on conversion to DwCA, timeslice sampling of GenBank, and georeferencing.

## Basic ideas

Index barcodes in Elasticsearch to provide BLAST-like search (based on 5-tuples), and also geospatial search. Build phylogenies on the fly using 5-tuple distance matrix and NJ.

## CouchDB

Use CouchDB to generate tiles of barcodes.

## Darwin Core and data management

Fetch sequences form BOLD and convert to Darwin Core, then have code to read Darwin Core and import into Elastic Search and CouchDB.

Means we can also add sequences from existing Darwin Core archives. Can also edit and version control DwCA files, such as georeferencing, publications, etc.

Also means we can send data to GBIF easily.

## Phylogenetic diversity

Could calculate phylogenetic diversity over a grid, say by indexing data in CouchDB to create a gridded view of sequences (e.g. one degree x one degree), then create data object for each grid cell that is put back into CouchDB and a view that computes the NJ there is used to compute overall diversity.

## Anthropocene

Could compare to Miraldo et al. who easier diversity WITHIN species but not between.

Miraldo, A., Li, S., Borregaard, M. K., Florez-Rodriguez, A., Gopalakrishnan, S., Rizvanovic, M., … Nogues-Bravo, D. (2016, September 29). An Anthropocene map of genetic diversity. Science. American Association for the Advancement of Science (AAAS). https://doi.org/10.1126/science.aaf4381

## Time slicing GenBank

Notes on how to get GenBank sequences from a given time interval and with particular fields (such as coordinates). See https://www.ncbi.nlm.nih.gov/books/NBK49540/ for list of NCBI search terms.

### Time period

NCBI recognises several types of date, such as **datetype=pdat** is publication date, **datetype=mdat** is revised date.

### Georeferenced sequences

Georeferenced sequences have the *lat_lon* field. Note that we can’t just search for *lat lon[prop]*, we need to prefix it with *src* so we have *src lat lon[prop]*, e.g.

https://www.ncbi.nlm.nih.gov/nuccore/?term=Fungi%5BOrganism%5D+AND+%22Internal+Transcribed+Spacer%22+AND+src+lat+lon%5Bprop%5D

This URL comes from email from Kessy Abarenkov kessy.abarenkov@ut.ee 

### Filtering out unwanted sequences

Gratton et al. in their supplementary material describe how to filter out sequences such as genome assemblies. Append the following in the query term **term**:

**AND ddbj embl genbank with limits[filt] NOT transcriptome[All Fields] NOT mRNA[filt] NOT TSA[All Fields] NOT scaffold[All Fields]**

### Keyword search

Amphibian barcodes in the last year by including *barcode[keyword]*

https://eutils.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?db=nucleotide&term=barcode[keyword]+txid8292[Organism:exp]&retmax=10&reldate=365&datetype=pdat


### Sequences in a time period with locality info

Georeferenced frogs within the last year:

https://eutils.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?db=nucleotide&term="Anura"[Organism]&retmax=10&reldate=365&datetype=pdat&AND+src+lat+lon%5Bprop%5D

### Sequences from a time slice

One month slice of all sequences with country but no lat_lon

https://eutils.ncbi.nlm.nih.gov/entrez/eutils/esearch.fcgi?db=nucleotide&term=ddbj embl genbank with limits[filt] NOT transcriptome[All Fields] NOT mRNA[filt] NOT TSA[All Fields] NOT scaffold[All Fields] AND src country[prop] NOT src lat lon[prop] AND 2017/01:2017/01/31[pdat]&retmax=1000

Sequences with *country* but now *lat_lon* are candidates for georeferencing.


### Examples

#### Sequence with country, locality in “isolate”

https://www.ncbi.nlm.nih.gov/nuccore/1134402056

## Georeferencing

For some background see also the repository https://github.com/rdmpage/Gratton_et_al_JBiogeogr_2016

### Examples

#### Sequence DQ523091
The first sequence in the amphibian data file is [DQ523091](https://www.ncbi.nlm.nih.gov/nuccore/DQ523091):
```
/isolate=“EtrivTSValviii9”
/country=“Peru: San Martin, Tarapoto”
```
If we use the geonames API  http://api.geonames.org/searchJSON/searchJSON?username=demo&q=Peru%3A%20San%20Martin%2C%20Tarapoto&maxRows=10 (you will need your own username) we get:

``` 
{
    “totalResultsCount”: 12,
    “geonames”: [
        {
            “adminCode1”: “22”,
            “lng”: “-76.37325”,
            “geonameId”: 6300820,
            “toponymName”: “Tarapoto”,
            “countryId”: “3932488”,
            “fcl”: “S”,
            “population”: 0,
            “countryCode”: “PE”,
            “name”: “Tarapoto”,
            “fclName”: “spot, building, farm”,
            “countryName”: “Peru”,
            “fcodeName”: “airport”,
            “adminName1”: “San Martín”,
            “lat”: “-6.50874”,
            “fcode”: “AIRP”
        },
        .
        .
        .
}
```
This gives the latitude and longitude values (-6.50874, -76.37325) reported in the data dump. Note that it is the airport.

In the paper [Genetic divergence and speciation in lowland and montane peruvian poison frogs](http://dx.doi.org/10.1016/j.ympev.2006.05.005) that published the sequence there is a specimen 
```
E. trivittatusP14	Tarapoto, San Martin, Peru	540 m	S 6.43066′ W 76.29034′
```
There doesn’t seem to be an obvious match between all the names in the table and the sequence in Genbank, but we should be able to extract some records. 


## References

Gratton, P., Marta, S., Bocksberger, G., Winter, M., Trucchi, E., & Kühl, H. (2016, July 4). A world of sequences: can we use georeferenced nucleotide databases for a robust automated phylogeography? Journal of Biogeography. Wiley-Blackwell. https://doi.org/10.1111/jbi.12786

Miraldo, A., Li, S., Borregaard, M. K., Florez-Rodriguez, A., Gopalakrishnan, S., Rizvanovic, M., … Nogues-Bravo, D. (2016, September 29). An Anthropocene map of genetic diversity. Science. American Association for the Advancement of Science (AAAS). https://doi.org/10.1126/science.aaf4381

Yang, K., & Zhang, L. (2008, January 10). Performance comparison between k-tuple distance and four model-based distances in phylogenetic tree reconstruction. Nucleic Acids Research. Oxford University Press (OUP). https://doi.org/10.1093/nar/gkn075


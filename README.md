# elastic-dna-barcodes
Using Elasticsearch to analyse DNA barcodes

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

## References

Gratton, P., Marta, S., Bocksberger, G., Winter, M., Trucchi, E., & Kühl, H. (2016, July 4). A world of sequences: can we use georeferenced nucleotide databases for a robust automated phylogeography? Journal of Biogeography. Wiley-Blackwell. https://doi.org/10.1111/jbi.12786

Miraldo, A., Li, S., Borregaard, M. K., Florez-Rodriguez, A., Gopalakrishnan, S., Rizvanovic, M., … Nogues-Bravo, D. (2016, September 29). An Anthropocene map of genetic diversity. Science. American Association for the Advancement of Science (AAAS). https://doi.org/10.1126/science.aaf4381

Yang, K., & Zhang, L. (2008, January 10). Performance comparison between k-tuple distance and four model-based distances in phylogenetic tree reconstruction. Nucleic Acids Research. Oxford University Press (OUP). https://doi.org/10.1093/nar/gkn075


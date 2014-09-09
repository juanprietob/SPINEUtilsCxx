:hide-uri-scheme:
:numbered:

= SPINEUtilsCXX

== Description 

This project has some of the back-end programs and utilities for SPINE

You can find the following utilities:

. SPINE contour manipulation.
.. ContoursIO: read, write.
.. Contour interpolation: bezier interpolation.
.. Contour values: area and perimeter calculation.

. Contour statistics:
.. link:https://www.google.com/url?sa=t&rct=j&q=&esrc=s&source=web&cd=4&cad=rja&uact=8&ved=0CEAQFjAD&url=http%3A%2F%2Fcs.unc.edu%2F~yihong%2Findex_files%2Fweighted_functional_boxplot_miccai_2013.pdf&ei=MBgGVLyyHpHzgwSygYH4Aw&usg=AFQjCNE_yaxhXWFeWQBfqHF3mXf5j4Dosg&sig2=C7oOwWRkO35fLrFi0DVjYA&bvm=bv.74115972,d.eXY[Weighted Functional Boxplot with Application to Statistical Atlas Construction]

. Image manipulation:
.. Resampling: Image ares converted from DICOM or nifti to RAS space.


== Requirements:
ITK, VTK, Xerces, CMake

ITK option for 'support version 3' must be on -> ITKV3_COMPATIBILITY ON

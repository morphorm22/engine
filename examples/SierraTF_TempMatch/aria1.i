# aria1.csm written by ocsmSave (v1.20)

# Constant, Design, and Output Parameters:
despmtr   length 6 lbound 4.7 ubound 10 initial 6

# Global Attributes:
set MeshLength .5
# Branches:
skbeg     0   0   0   1
   skvar     xy   0.000000;0.000000;0.000000;1.000000;0.000000;0.000000;2.000000;0.000000;0.000000;3.000000;0.000000;0.000000;4.000000;0.000000;0.000000;5.000000;0.000000;0.000000;5.000000;1.000000;0.000000;0.000000;1.000000;0.000000;
   skcon     X   1   -1  0
   skcon     Y   1   -1  0
   skcon     H   1   2  0
   skcon     H   2   3  0
   skcon     H   3   4  0
   skcon     H   4   5  0
   skcon     H   5   6  0
   skcon     V   6   7  0
   skcon     H   7   8  0
   skcon     V   8   1  0
   skcon     X   6   -1  length
   skcon     Y   7   -1  1
   skcon     X   2   -1  1
   skcon     X   3   -1  2
   skcon     X   4   -1  3
   skcon     X   5   -1  4
   linseg    ::x[2]   ::y[2]   0
   linseg    ::x[3]   ::y[3]   0
   linseg    ::x[4]   ::y[4]   0
   linseg    ::x[5]   ::y[5]   0
   linseg    ::x[6]   ::y[6]   0
   linseg    ::x[7]   ::y[7]   0
   linseg    ::x[8]   ::y[8]   0
   linseg    ::x[1]   ::y[1]   0
skend     0
extrude   0   0   1
select    body   11
select    face   11   2   1
attribute capsGroup   $search_nodeset
select    body   11
select    face   11   10   1
attribute capsGroup   $temp_sideset
select    body   11
select    face   11   8   1
attribute capsGroup   $flux_sideset

end

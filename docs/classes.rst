API
=======================
PyOpenCAP exposes two OpenCAP classes to Python: System and Projected_CAP. All data returned
by the methods of these objects is passed by value, which means the underlying C++ code
retains ownership of the original data. Data passed to these objects from Python is passed
by reference, but is not modified in any way by the underlying C++ code.

.. toctree::
   :maxdepth: 1
   
   classes/System.rst
   classes/Projected_CAP.rst
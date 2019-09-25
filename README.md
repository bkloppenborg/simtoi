SIMTOI
======

[!SIMTOI Screenshot](doc/simtoi-screenshot.png)

The SImulation and Modeling Tool for Optical Interferometry (SIMTOI) is an open
source (GPL v3) software package for simulating and modeling optical interferometric
observations of complex astrophysical sources that are not easily described
by analytic expressions.

*NOTE* SIMTOI is no longer under active development. We will; however, fix bugs when 
they are discovered and accept pull request that implement new features.

## Installation Instructions

At present the only method of installing SIMTOI is compilation from source.
Please see the
[instructions on the wiki](https://github.com/bkloppenborg/simtoi/wiki/Installation)
for further information.

## Reporting issues

If you encounter any issues with the code, or have requests for new features,
please open a new ticket on the GitHub
[issue tracker](https://github.com/bkloppenborg/simtoi/issues)
You can also send any questions/issues to brian@kloppenborg.net

Please try and resolve installation issues with your system administrator,
as they are likely due to issues with dependencies - however, if you think you
have encountered a genuine issue in the installation, please do report it.

## Contributing

In addition to reporting issues with the code or the documentation, code and
documentation contributions/patches are welcome. Please read the
[following page](https://github.com/bkloppenborg/simtoi/blob/master/CONTRIBUTING.md)
for more details on how to contribute code.

## Acknowledgements

SIMTOI includes the following libraries as part of its distribution:
* [OpenCL Interferometry Library (liboi)](https://github.com/bkloppenborg/liboi)
* [textio](https://github.com/bkloppenborg/textio)
* [ccoifits](https://github.com/bkloppenborg/ccoifits).
* [json-cpp](https://github.com/open-source-parsers/jsoncpp)
* [levmar](http://www.ics.forth.gr/~lourakis/levmar/)

SIMTOI supports, but our repositories do not include,

* [MultiNest](http://ccpforge.cse.rl.ac.uk/gf/project/multinest/) A bayesian nested
  sampling minimzation engine.

## Licensing

SIMTOI is free software, distributed under the
[GNU Lesser General Public License (Version 3)](<http://www.gnu.org/licenses/lgpl.html).

If you use this software as part of a scientific publication, we request that
you cite its use as follows:

Kloppenborg, B.; Baron, F. (2012) "SIMTOI: SImulation and Modeling Tool for
Optical Interferometry" (Version X).  Available from
<https://github.com/bkloppenborg/simtoi>.

Kloppenborg, B.; Baron, F. (2012), "LibOI: The OpenCL Interferometry Library"
(Version X). Available from  <https://github.com/bkloppenborg/liboi>.

If you use the _levmar_ minmizer, see their instructions for including
[a suitable reference](http://www.ics.forth.gr/~lourakis/levmar/bibentry.html).

If you use the _MultiNest_ minimizer see reference instructions on the
[_MultiNest_ website](http://ccpforge.cse.rl.ac.uk/gf/project/multinest/)


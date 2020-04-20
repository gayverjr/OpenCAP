/*
 * readMolcasHDF5.h
 *
 *  Created on: Mar 15, 2020
 *      Author: JG
 */
#include <armadillo>
#ifndef READMOLCASHDF5_H_
#define READMOLCASHDF5_H_

std::array<std::vector<std::vector<arma::mat>>,2> read_rassi_HDF5(std::string dmat_filename);

#endif /* READMOLCASHDF5_H_ */

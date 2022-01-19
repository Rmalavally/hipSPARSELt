/* ************************************************************************
 * Copyright (c) 2021-2022 Advanced Micro Devices, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * ************************************************************************ */

/*! \file
 *  \brief rocsparselt-auxiliary.h provides auxilary functions in rocsparselt
 */

#pragma once
#ifndef _ROCSPARSELT_AUXILIARY_H_
#define _ROCSPARSELT_AUXILIARY_H_

#include <stdint.h>

#include "rocsparselt-export.h"
#include "rocsparselt-types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! \ingroup aux_module
 *  \brief Create a rocsparselt handle
 *
 *  \details
 *  \p rocsparselt_init creates the rocSPARSELt library context. It must be
 *  initialized before any other rocSPARSELt API function is invoked and must be passed to
 *  all subsequent library function calls. The handle should be destroyed at the end
 *  using rocsparselt_destroy_handle().
 *
 *  @param[out]
 *  handle  the pointer to the handle to the rocSPARSELt library context.
 *
 *  \retval rocsparse_status_success the initialization succeeded.
 *  \retval rocsparse_status_invalid_handle \p handle pointer is invalid.
 *  \retval rocsparse_status_internal_error an internal error occurred.
 */
ROCSPARSELT_EXPORT
rocsparse_status rocsparselt_init(rocsparselt_handle* handle);

/*! \ingroup aux_module
 *  \brief Destroy a rocsparselt handle
 *
 *  \details
 *  \p rocsparselt_destroy destroys the rocSPARSELt library context and releases all
 *  resources used by the rocSPARSELt library.
 *
 *  @param[in]
 *  handle  the handle to the rocSPARSELt library context.
 *
 *  \retval rocsparse_status_success the operation completed successfully.
 *  \retval rocsparse_status_invalid_handle \p handle is invalid.
 *  \retval rocsparse_status_internal_error an internal error occurred.
 */
ROCSPARSELT_EXPORT
rocsparse_status rocsparselt_destroy(const rocsparselt_handle handle);

/*! \ingroup aux_module
 *  \brief Create a descriptor for dense matrix
 *  \details
 *  \p rocsparselt_dense_descr_init creates a matrix descriptor It initializes
 *  \ref rocsparselt_matrix_type to \ref rocsparselt_matrix_type_dense and
 *  It should be destroyed at the end using rocsparselt_mat_descr_destroy().
 *
 *  @param[out]
 *  matDescr   the pointer to the dense matrix descriptor
 *
 *  \retval rocsparse_status_success the operation completed successfully.
 *  \retval rocsparse_status_invalid_pointer \p descr pointer is invalid.
 *  \retval rocsparse_status_invalid_value
 */
ROCSPARSELT_EXPORT
rocsparse_status rocsparselt_dense_descr_init(const rocsparselt_handle handle,
                                              rocsparselt_mat_descr*   matDescr,
                                              int64_t                  rows,
                                              int64_t                  cols,
                                              int64_t                  ld,
                                              uint32_t                 alignment,
                                              rocsparselt_datatype     valueType,
                                              rocsparse_order          order);

/*! \ingroup aux_module
 *  \brief Create a descriptor for structured matrix
 *  \details
 *  \p rocsparselt_dense_descr_init creates a matrix descriptor It initializes
 *  \ref rocsparselt_matrix_type to \ref rocsparselt_matrix_type_structured and
 *  It should be destroyed at the end using rocsparselt_mat_descr_destroy().
 *
 *  @param[out]
 *  matDescr   the pointer to the dense matrix descriptor
 *
 *  \retval rocsparse_status_success the operation completed successfully.
 *  \retval rocsparse_status_invalid_pointer \p descr pointer is invalid.
 *  \retval rocsparse_status_invalid_value
 */
ROCSPARSELT_EXPORT
rocsparse_status rocsparselt_structured_descr_init(const rocsparselt_handle handle,
                                                   rocsparselt_mat_descr*   matDescr,
                                                   int64_t                  rows,
                                                   int64_t                  cols,
                                                   int64_t                  ld,
                                                   uint32_t                 alignment,
                                                   rocsparselt_datatype     valueType,
                                                   rocsparse_order          order,
                                                   rocsparselt_sparsity     sparsity);

/*! \ingroup aux_module
 *  \brief Destroy a matrix descriptor
 *
 *  \details
 *  \p rocsparselt_mat_descr_destroy destroys a matrix descriptor and releases all
 *  resources used by the descriptor
 *
 *  @param[in]
 *  descr   the matrix descriptor
 *
 *  \retval rocsparse_status_success the operation completed successfully.
 *  \retval rocsparse_status_invalid_pointer \p descr is invalid.
 */
ROCSPARSELT_EXPORT
rocsparse_status rocsparselt_mat_descr_destroy(const rocsparselt_mat_descr descr);

/*! \ingroup aux_module
 *  \brief Specify the matrix attribute of a matrix descriptor
 *
 *  \details
 *  \p rocsparselt_mat_descr_set_attribute sets the value of the specified attribute belonging
 *  to matrix descr such as number of batches and their stride.
 *
 *  @param[inout]
 *  matDescr        the matrix descriptor
 *  @param[in]
 *  handle          the rocsparselt handle
 *  matAttribute    \ref rocsparselt_mat_num_batches, \ref rocsparselt_mat_batch_stride.
 *  data            pointer to the value to which the specified attribute will be set.
 *  dataSize        size in bytes of the attribute value used for verification.
 *
 *  \retval rocsparse_status_success the operation completed successfully.
 *  \retval rocsparse_status_invalid_handle \p handle or \p descr pointer is invalid.
 *  \retval rocsparse_status_invalid_pointer \p data pointer is invalid.
 *  \retval rocsparse_status_invalid_value \p rocsparselt_mat_descr_attribute is invalid.
 */
ROCSPARSELT_EXPORT
rocsparse_status rocsparselt_mat_descr_set_attribute(const rocsparselt_handle        handle,
                                                     rocsparselt_mat_descr           matDescr,
                                                     rocsparselt_mat_descr_attribute matAttribute,
                                                     const void*                     data,
                                                     size_t                          dataSize);

/*! \ingroup aux_module
 *  \brief Get the matrix type of a matrix descriptor
 *
 *  \details
 *  \p rocsparselt_mat_descr_get_attribute returns the matrix attribute of a matrix descriptor
 *
 *  @param[inout]
 *  data            the memory address containing the attribute value retrieved by this function
 *
 *  @param[in]
 *  handle          the rocsparselt handle
 *  matAttribute    \ref rocsparselt_mat_num_batches, \ref rocsparselt_mat_batch_stride.
 *  matDescr        the matrix descriptor
 *  dataSize        size in bytes of the attribute value used for verification.
 *
 *  \retval rocsparse_status_success the operation completed successfully.
 *  \retval rocsparse_status_invalid_pointer \p handle, \p descr or \p data pointer is invalid.
 *  \retval rocsparse_status_invalid_value \p rocsparselt_mat_descr_attribute is invalid.
 */
ROCSPARSELT_EXPORT
rocsparse_status rocsparselt_mat_descr_get_attribute(const rocsparselt_handle        handle,
                                                     const rocsparselt_mat_descr     matDescr,
                                                     rocsparselt_mat_descr_attribute matAttribute,
                                                     void*                           data,
                                                     size_t                          dataSize);

/*! \ingroup aux_module
 *  \brief  Initializes the matrix multiplication descriptor.
 *
 *  \details
 *  \p rocsparselt_matmul_descr_init creates a matrix multiplication descriptor.
 *  It should be destroyed at the end using rocsparselt_matmul_descr_destroy().
 *
 *  @param[inout]
 *  matmulDescr     the matrix multiplication descriptor
 *  @param[in]
 *  handle   the rocsparselt handle
 *  opA
 *  opB
 *  matA     the matrix descriptor
 *  matB     the matrix descriptor
 *  matC     the matrix descriptor
 *  matD     the matrix descriptor
 *  computeType        size in bytes of the attribute value used for verification.
 *
 *  \retval rocsparse_status_success the operation completed successfully.
 *  \retval rocsparse_status_invalid_pointer \p matmulDescr pointer is invalid.
 *  \retval rocsparse_status_invalid_value \p computeType is invalid.
 */
ROCSPARSELT_EXPORT
rocsparse_status rocsparselt_matmul_descr_init(const rocsparselt_handle  handle,
                                               rocsparselt_matmul_descr* matmulDescr,
                                               rocsparse_operation       opA,
                                               rocsparse_operation       opB,
                                               rocsparselt_mat_descr     matA,
                                               rocsparselt_mat_descr     matB,
                                               rocsparselt_mat_descr     matC,
                                               rocsparselt_mat_descr     matD,
                                               rocsparselt_compute_type  computeType);

/*! \ingroup aux_module
 *  \brief Destroy a matrix multiplication descriptor
 *
 *  \details
 *  \p rocsparselt_mat_descr_destroy destroys a multiplication matrix descr.
 *
 *  @param[in]
 *  descr   the matrix multiplication descriptor
 *
 *  \retval rocsparse_status_success the operation completed successfully.
 *  \retval rocsparse_status_invalid_value \p descr is invalid.
 */
ROCSPARSELT_EXPORT
rocsparse_status rocsparselt_matmul_descr_destroy(const rocsparselt_matmul_descr descr);

/*! \ingroup aux_module
 *  \brief Initializes the algorithm selection descriptor
 *  \details
 *  \p rocsparselt_matmul_alg_selection_init creates a algorithm selection descriptor.
 *  It should be destroyed at the end using rocsparselt_matmul_alg_selection_destroy().
 *
 *  @param[out]
 *  algSelection the pointer to the algorithm selection descriptor
 *
 *  \retval rocsparse_status_success the operation completed successfully.
 *  \retval rocsparse_status_invalid_pointer \p algSelection pointer is invalid.
 *  \retval rocsparse_status_invalid_value
 */
ROCSPARSELT_EXPORT
rocsparse_status
    rocsparselt_matmul_alg_selection_init(const rocsparselt_handle          handle,
                                          rocsparselt_matmul_alg_selection* algSelection,
                                          const rocsparselt_matmul_descr    matmulDescr,
                                          rocsparselt_matmul_alg            alg);
/*! \ingroup aux_module
 *  \brief Destroy a algorithm selection descriptor
 *
 *  \details
 *  \p rocsparselt_matmul_alg_selection_destroy destroys a algorithm selection descriptor
 *  and releases all resources used by the descriptor
 *
 *  @param[in]
 *  algSelection   the algorithm selection descriptor
 *
 *  \retval rocsparse_status_success the operation completed successfully.
 *  \retval rocsparse_status_invalid_pointer \p algSelection is invalid.
 */
ROCSPARSELT_EXPORT
rocsparse_status
    rocsparselt_matmul_alg_selection_destroy(const rocsparselt_matmul_alg_selection algSelection);

/*! \ingroup aux_module
 *  \brief Specify the algorithm attribute of a algorithm selection descriptor
 *
 *  \details
 *  \p rocsparselt_matmul_alg_set_attribute sets the value of the specified attribute
 *  belonging to algorithm selection descriptor.
 *
 *  @param[inout]
 *  algSelection    the algorithm selection descriptor
 *  @param[in]
 *  handle          the rocsparselt handle
 *  attribute
 *  data            pointer to the value to which the specified attribute will be set.
 *  dataSize        size in bytes of the attribute value used for verification.
 *
 *  \retval rocsparse_status_success the operation completed successfully.
 *  \retval rocsparse_status_invalid_handle \p handle or \p algSelection pointer is invalid.
 *  \retval rocsparse_status_invalid_pointer \p data pointer is invalid.
 *  \retval rocsparse_status_invalid_value \p rocsparselt_matmul_alg_attribute is invalid.
 */
ROCSPARSELT_EXPORT
rocsparse_status rocsparselt_matmul_alg_set_attribute(const rocsparselt_handle         handle,
                                                      rocsparselt_matmul_alg_selection algSelection,
                                                      rocsparselt_matmul_alg_attribute attribute,
                                                      const void*                      data,
                                                      size_t                           dataSize);

/*! \ingroup aux_module
 *  \brief Get the specific algorithm attribute from algorithm selection descriptor
 *
 *  \details
 *  \p rocsparselt_matmul_alg_get_attribute returns the value of the queried attribute belonging
 *  to algorithm selection descriptor.
 *
 *  @param[inout]
 *  data            the memory address containing the attribute value retrieved by this function
 *
 *  @param[in]
 *  handle          the rocsparselt handle
 *  algSelection    the algorithm selection descriptor
 *  dataSize        size in bytes of the attribute value used for verification.
 *
 *  \retval rocsparse_status_success the operation completed successfully.
 *  \retval rocsparse_status_invalid_handle \p handle or \p algSelection pointer is invalid.
 *  \retval rocsparse_status_invalid_pointer \p data pointer is invalid.
 *  \retval rocsparse_status_invalid_value \p rocsparselt_matmul_alg_attribute is invalid.
 */
ROCSPARSELT_EXPORT
rocsparse_status rocsparselt_matmul_alg_get_attribute(const rocsparselt_handle         handle,
                                                      rocsparselt_matmul_alg_selection algSelection,
                                                      rocsparselt_matmul_alg_attribute attribute,
                                                      void*                            data,
                                                      size_t                           dataSize);

/*! \ingroup aux_module
 *  \brief Initializes the matrix multiplication plan descriptor
 *  \details
 *  \p rocsparselt_matmul_plan_init creates a matrix multiplication plan descriptor.
 *  It should be destroyed at the end using rocsparselt_matmul_matmul_plan_destroy().
 *
 *  @param[out]
 *  plan the pointer to the matrix multiplication plan descriptor
 *
 *  \retval rocsparse_status_success the operation completed successfully.
 *  \retval rocsparse_status_invalid_pointer \p plan pointer is invalid.
 *  \retval rocsparse_status_invalid_value
 */
ROCSPARSELT_EXPORT
rocsparse_status rocsparselt_matmul_plan_init(const rocsparselt_handle               handle,
                                              rocsparselt_matmul_plan*               plan,
                                              const rocsparselt_matmul_descr         matmulDescr,
                                              const rocsparselt_matmul_alg_selection algSelection,
                                              size_t                                 workspaceSize);

/*! \ingroup aux_module
 *  \brief Destroy a matrix multiplication plan descriptor
 *  \details
 *  \p rocsparselt_matmul_plan_destroy releases the resources used by an instance
 *  of the matrix multiplication plan. This function is the last call with a specific plan
 *  instance.
 *
 *  @param[in]
 *  plan the matrix multiplication plan descriptor
 *
 *  \retval rocsparse_status_success the operation completed successfully.
 *  \retval rocsparse_status_invalid_pointer \p plan pointer is invalid.
 *  \retval rocsparse_status_invalid_value
 */
ROCSPARSELT_EXPORT
rocsparse_status rocsparselt_matmul_plan_destroy(const rocsparselt_matmul_plan plan);

#ifdef __cplusplus
}
#endif

#endif /* _ROCSPARSELT_AUXILIARY_H_ */

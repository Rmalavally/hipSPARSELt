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

#include "definitions.h"
#include "handle.h"
#include "rocsparselt.h"
#include "utility.h"

#include <hip/hip_runtime_api.h>

#define TO_STR2(x) #x
#define TO_STR(x) TO_STR2(x)

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************
 * \brief rocsparselt_handle is a structure holding the rocsparselt library context.
 * It must be initialized using rocsparselt_init()
 * and the returned handle must be passed
 * to all subsequent library function calls.
 * It should be destroyed at the end using rocsparselt_destroy().
 *******************************************************************************/
rocsparse_status rocsparselt_init(rocsparselt_handle* handle)
{
    // Check if handle is valid
    if(handle == nullptr)
    {
        return rocsparse_status_invalid_handle;
    }
    else
    {
        *handle = nullptr;
        // Allocate
        try
        {
            *handle = new _rocsparselt_handle();
            log_trace(*handle, "rocsparselt_init");
        }
        catch(const rocsparse_status& status)
        {
            return status;
        }
        return rocsparse_status_success;
    }
}

/********************************************************************************
 * \brief destroy handle
 *******************************************************************************/
rocsparse_status rocsparselt_destroy(const rocsparselt_handle handle)
{
    log_trace(handle, "rocsparse_destroy");
    // Destruct
    try
    {
        delete handle;
    }
    catch(const rocsparse_status& status)
    {
        return status;
    }
    return rocsparse_status_success;
}

/********************************************************************************
 * \brief rocsparse_mat_descr is a structure holding the rocsparselt matrix
 * content. It must be initialized using rocsparselt_dense_descr_init() or
 * rocsparselt_structured_descr_init()  and the retured handle must be passed
 * to all subsequent library function calls that involve the matrix.
 * It should be destroyed at the end using rocsparselt_mat_descr_destroy().
 *******************************************************************************/
rocsparse_status rocsparselt_dense_descr_init(const rocsparselt_handle handle,
                                              rocsparselt_mat_descr*   matDescr,
                                              int64_t                  rows,
                                              int64_t                  cols,
                                              int64_t                  ld,
                                              uint32_t                 alignment,
                                              rocsparselt_datatype     valueType,
                                              rocsparse_order          order)
{
    // Check if matDescr is valid
    if(handle == nullptr)
    {
        return rocsparse_status_invalid_handle;
    }
    else if(matDescr == nullptr)
    {
        return rocsparse_status_invalid_pointer;
    }
    else if(order != rocsparse_order_column)
    {
        return rocsparse_status_not_implemented;
    }
    else
    {
        *matDescr = nullptr;
        // Allocate
        try
        {
            *matDescr              = new _rocsparselt_mat_descr();
            (*matDescr)->m_type    = rocsparselt_matrix_type_dense;
            (*matDescr)->m         = rows;
            (*matDescr)->n         = cols;
            (*matDescr)->ld        = ld;
            (*matDescr)->alignment = alignment;
            (*matDescr)->type      = valueType;
            (*matDescr)->order     = order;
            log_trace(handle, "rocsparselt_dense_descr_init");
        }
        catch(const rocsparse_status& status)
        {
            return status;
        }
        return rocsparse_status_success;
    }
}

/********************************************************************************
 * \brief rocsparse_mat_descr is a structure holding the rocsparselt matrix
 * content. It must be initialized using rocsparselt_dense_descr_init() or
 * rocsparselt_structured_descr_init()  and the retured handle must be passed
 * to all subsequent library function calls that involve the matrix.
 * It should be destroyed at the end using rocsparselt_mat_descr_destroy().
 *******************************************************************************/
rocsparse_status rocsparselt_structured_descr_init(const rocsparselt_handle handle,
                                                   rocsparselt_mat_descr*   matDescr,
                                                   int64_t                  rows,
                                                   int64_t                  cols,
                                                   int64_t                  ld,
                                                   uint32_t                 alignment,
                                                   rocsparselt_datatype     valueType,
                                                   rocsparse_order          order,
                                                   rocsparselt_sparsity     sparsity)

{
    // Check if matDescr is valid
    if(handle == nullptr)
    {
        return rocsparse_status_invalid_handle;
    }
    else if(matDescr == nullptr)
    {
        return rocsparse_status_invalid_pointer;
    }
    else if(order != rocsparse_order_column)
    {
        return rocsparse_status_not_implemented;
    }
    else
    {
        *matDescr = nullptr;
        // Allocate
        try
        {
            *matDescr              = new _rocsparselt_mat_descr();
            (*matDescr)->m_type    = rocsparselt_matrix_type_structured;
            (*matDescr)->m         = rows;
            (*matDescr)->n         = cols;
            (*matDescr)->ld        = ld;
            (*matDescr)->alignment = alignment;
            (*matDescr)->type      = valueType;
            (*matDescr)->order     = order;
            (*matDescr)->sparsity  = sparsity;
            log_trace(handle, "rocsparselt_structured_descr_init");
        }
        catch(const rocsparse_status& status)
        {
            return status;
        }
        return rocsparse_status_success;
    }
}

/********************************************************************************
 * \brief destroy matrix descriptor
 *******************************************************************************/
rocsparse_status rocsparselt_mat_descr_destroy(const rocsparselt_mat_descr matDescr)
{
    if(matDescr == nullptr)
        return rocsparse_status_invalid_pointer;
    // Destruct
    try
    {
        constexpr size_t attrs = sizeof(matDescr->attributes) / sizeof(matDescr->attributes[0]);
        for(int i = 0; i < attrs; i++)
        {
            if(matDescr->attributes[i].data_size > 0)
                free(matDescr->attributes[i].data);
        }
        delete matDescr;
    }
    catch(const rocsparse_status& status)
    {
        return status;
    }
    return rocsparse_status_success;
}

/********************************************************************************
 * \brief sets the value of the specified attribute belonging to matrix descriptor
 * such as number of batches and their stride.
 *******************************************************************************/
rocsparse_status rocsparselt_mat_descr_set_attribute(const rocsparselt_handle        handle,
                                                     rocsparselt_mat_descr           matDescr,
                                                     rocsparselt_mat_descr_attribute matAttribute,
                                                     const void*                     data,
                                                     size_t                          dataSize)

{
    if(handle == nullptr)
    {
        return rocsparse_status_invalid_handle;
    }
    else if(data == nullptr || matDescr == nullptr)
    {
        return rocsparse_status_invalid_pointer;
    }
    else if(dataSize <= 0)
    {
        return rocsparse_status_invalid_value;
    }
    else
    {
        // Allocate
        try
        {
            if(matDescr->attributes[matAttribute].data != nullptr
               && matDescr->attributes[matAttribute].data_size > 0)
            {
                free(matDescr->attributes[matAttribute].data);
                matDescr->attributes[matAttribute].data = nullptr;
            }
            if(matDescr->attributes[matAttribute].data == nullptr)
                matDescr->attributes[matAttribute].data = malloc(dataSize);
            matDescr->attributes[matAttribute].data_size = dataSize;
            memcpy(matDescr->attributes[matAttribute].data, data, dataSize);
            log_trace(handle, "rocsparselt_mat_descr_set_attribute");
        }
        catch(const rocsparse_status& status)
        {
            return status;
        }
        return rocsparse_status_success;
    }
}

/********************************************************************************
 * \brief sets the value of the specified attribute belonging to matrix descriptor
 * such as number of batches and their stride.
 *******************************************************************************/
rocsparse_status rocsparselt_mat_descr_get_attribute(const rocsparselt_handle        handle,
                                                     const rocsparselt_mat_descr     matDescr,
                                                     rocsparselt_mat_descr_attribute matAttribute,
                                                     void*                           data,
                                                     size_t                          dataSize)
{
    // Check if matmulDescr is valid
    if(handle == nullptr)
    {
        return rocsparse_status_invalid_handle;
    }
    else if(data == nullptr || matDescr == nullptr)
    {
        return rocsparse_status_invalid_pointer;
    }
    else if(dataSize <= 0)
    {
        return rocsparse_status_invalid_value;
    }
    else
    {
        try
        {
            memcpy(data,
                   matDescr->attributes[matAttribute].data,
                   std::min(dataSize, matDescr->attributes[matAttribute].data_size));
            log_trace(handle, "rocsparselt_mat_descr_get_attribute");
        }
        catch(const rocsparse_status& status)
        {
            return status;
        }
        return rocsparse_status_success;
    }
}

/********************************************************************************
 * \brief
 *******************************************************************************/
rocsparse_status rocsparselt_matmul_descr_init(const rocsparselt_handle  handle,
                                               rocsparselt_matmul_descr* matmulDescr,
                                               rocsparse_operation       opA,
                                               rocsparse_operation       opB,
                                               rocsparselt_mat_descr     matA,
                                               rocsparselt_mat_descr     matB,
                                               rocsparselt_mat_descr     matC,
                                               rocsparselt_mat_descr     matD,
                                               rocsparselt_compute_type  computeType)
{
    // Check if matmulDescr is valid
    if(handle == nullptr)
    {
        return rocsparse_status_invalid_handle;
    }
    else if(matmulDescr == nullptr || matA == nullptr || matB == nullptr || matC == nullptr
            || matD == nullptr)
    {
        return rocsparse_status_invalid_pointer;
    }
    else if(matA->m_type != rocsparselt_matrix_type_structured
            || matB->m_type != rocsparselt_matrix_type_dense)
    {
        return rocsparse_status_not_implemented;
    }
    else
    {
        *matmulDescr = nullptr;
        // Allocate
        try
        {
            if(matA->type != matB->type || matA->type != matC->type || matA->type != matD->type)
                throw rocsparse_status_invalid_value;

            switch(matA->type)
            {
            case rocsparselt_datatype_bf16_r:
            case rocsparselt_datatype_bf8_r:
            case rocsparselt_datatype_f16_r:
            case rocsparselt_datatype_f8_r:
                if(computeType != rocsparselt_compute_f32)
                    throw rocsparse_status_invalid_value;
                break;
            case rocsparselt_datatype_i8_r:
                if(computeType != rocsparselt_compute_i32)
                    throw rocsparse_status_invalid_value;
                break;
            }

            *matmulDescr                 = new _rocsparselt_matmul_descr();
            (*matmulDescr)->op_A         = opA;
            (*matmulDescr)->op_B         = opB;
            (*matmulDescr)->matrix_A     = matA;
            (*matmulDescr)->matrix_B     = matB;
            (*matmulDescr)->matrix_C     = matC;
            (*matmulDescr)->matrix_D     = matD;
            (*matmulDescr)->compute_type = computeType;
            log_trace(handle, "rocsparselt_matmul_descr_init");
        }
        catch(const rocsparse_status& status)
        {
            return status;
        }
        return rocsparse_status_success;
    }
}

/********************************************************************************
 * \brief destroy matrix multiplication descriptor
 *******************************************************************************/
rocsparse_status rocsparselt_matmul_descr_destroy(const rocsparselt_matmul_descr matmulDescr)
{
    if(matmulDescr == nullptr)
    {
        return rocsparse_status_invalid_pointer;
    }

    // Destruct
    try
    {
        if(matmulDescr->bias_pointer.data_size > 0 && matmulDescr->bias_pointer.data != nullptr)
            free(matmulDescr->bias_pointer.data);
        delete matmulDescr;
    }
    catch(const rocsparse_status& status)
    {
        return status;
    }
    return rocsparse_status_success;
}

/********************************************************************************
 * \brief sets the value of the specified attribute belonging to matrix multiplication
 * descriptor.
 *******************************************************************************/
rocsparse_status
    rocsparselt_matmul_descr_set_attribute(const rocsparselt_handle           handle,
                                           rocsparselt_matmul_descr           matmulDescr,
                                           rocsparselt_matmul_descr_attribute matmulAttribute,
                                           const void*                        data,
                                           size_t                             dataSize)
{
    // Check if matmulDescr is valid
    if(matmulDescr == nullptr)
    {
        return rocsparse_status_invalid_handle;
    }
    else if(data == nullptr)
    {
        return rocsparse_status_invalid_pointer;
    }
    else if(dataSize <= 0)
    {
        return rocsparse_status_invalid_value;
    }
    else
    {
        // Allocate
        try
        {

            switch(matmulAttribute)
            {
            case rocsparselt_matmul_activation_relu:
                if(sizeof(int) == dataSize)
                    memcpy(&matmulDescr->activation_relu, data, sizeof(int));
                else
                    return rocsparse_status_invalid_value;
                break;
            case rocsparselt_matmul_activation_relu_upperbound:
                if(sizeof(float) == dataSize)
                    memcpy(&matmulDescr->activation_relu_upperbound, data, sizeof(float));
                else
                    return rocsparse_status_invalid_value;
                break;
            case rocsparselt_matmul_activation_relu_threshold:
                if(sizeof(float) == dataSize)
                    memcpy(&matmulDescr->activation_relu_threshold, data, sizeof(float));
                else
                    return rocsparse_status_invalid_value;
                break;
            case rocsparselt_matmul_activation_gelu:
                if(sizeof(int) == dataSize)
                    memcpy(&matmulDescr->activation_gelu, data, sizeof(int));
                else
                    return rocsparse_status_invalid_value;
                break;
            case rocsparselt_matmul_bias_pointer:
                if(matmulDescr->bias_pointer.data != nullptr
                   && matmulDescr->bias_pointer.data_size != dataSize)
                {
                    free(matmulDescr->bias_pointer.data);
                    matmulDescr->bias_pointer.data = nullptr;
                }

                if(dataSize <= 0)
                    return rocsparse_status_invalid_value;

                //TODO Check the bias vector size is equal to the number of rows of the output matrix D.

                if(matmulDescr->bias_pointer.data == nullptr)
                {
                    matmulDescr->bias_pointer.data = malloc(dataSize);
                }

                matmulDescr->bias_pointer.data_size = dataSize;
                memcpy(matmulDescr->bias_pointer.data, data, dataSize);
                break;
            case rocsparselt_matmul_bias_stride:
                if(sizeof(int64_t) == dataSize)
                    memcpy(&matmulDescr->bias_stride, data, sizeof(int64_t));
                else
                    return rocsparse_status_invalid_value;
                break;
            }
            log_trace(handle, "rocsparselt_matmul_descr_set_attribute");
        }
        catch(const rocsparse_status& status)
        {
            return status;
        }
        return rocsparse_status_success;
    }
}

/********************************************************************************
 * \brief sets the value of the specified attribute belonging to matrix descriptor
 * such as number of batches and their stride.
 *******************************************************************************/
rocsparse_status
    rocsparselt_matmul_descr_get_attribute(const rocsparselt_handle           handle,
                                           rocsparselt_matmul_descr           matmulDescr,
                                           rocsparselt_matmul_descr_attribute matmulAttribute,
                                           void*                              data,
                                           size_t                             dataSize)

{
    // Check if matmulDescr is valid
    if(matmulDescr == nullptr)
    {
        return rocsparse_status_invalid_handle;
    }
    else if(data == nullptr)
    {
        return rocsparse_status_invalid_pointer;
    }
    else
    {
        try
        {
            switch(matmulAttribute)
            {
            case rocsparselt_matmul_activation_relu:
                if(dataSize < sizeof(int))
                    return rocsparse_status_invalid_value;
                memcpy(data, &matmulDescr->activation_relu, sizeof(int));
                break;
            case rocsparselt_matmul_activation_relu_upperbound:
                if(dataSize < sizeof(float))
                    return rocsparse_status_invalid_value;
                memcpy(data, &matmulDescr->activation_relu_upperbound, sizeof(float));
                break;
            case rocsparselt_matmul_activation_relu_threshold:
                if(dataSize < sizeof(float))
                    return rocsparse_status_invalid_value;
                memcpy(data, &matmulDescr->activation_relu_threshold, sizeof(float));
                break;
            case rocsparselt_matmul_activation_gelu:
                if(dataSize < sizeof(int))
                    return rocsparse_status_invalid_value;
                memcpy(data, &matmulDescr->activation_gelu, sizeof(int));
                break;
            case rocsparselt_matmul_bias_pointer:
                if(dataSize < matmulDescr->bias_pointer.data_size)
                    return rocsparse_status_invalid_value;
                memcpy(data, matmulDescr->bias_pointer.data, matmulDescr->bias_pointer.data_size);
                break;
            case rocsparselt_matmul_bias_stride:
                if(dataSize < sizeof(int64_t))
                    return rocsparse_status_invalid_value;
                memcpy(data, &matmulDescr->bias_stride, sizeof(int64_t));
                break;
            }
            log_trace(handle, "rocsparselt_matmul_descr_get_attribute");
        }
        catch(const rocsparse_status& status)
        {
            return status;
        }
        return rocsparse_status_success;
    }
}

/********************************************************************************
 * \brief
 *******************************************************************************/
rocsparse_status
    rocsparselt_matmul_alg_selection_init(const rocsparselt_handle          handle,
                                          rocsparselt_matmul_alg_selection* algSelection,
                                          const rocsparselt_matmul_descr    matmulDescr,
                                          rocsparselt_matmul_alg            alg)
{
    // Check if algSelection is valid
    if(handle == nullptr)
    {
        return rocsparse_status_invalid_handle;
    }
    else if(algSelection == nullptr || matmulDescr == nullptr)
    {
        return rocsparse_status_invalid_pointer;
    }
    else
    {
        *algSelection = nullptr;
        // Allocate
        try
        {
            *algSelection        = new _rocsparselt_matmul_alg_selection();
            (*algSelection)->alg = alg;
            log_trace(handle, "rocsparselt_matmul_alg_selection_init");
        }
        catch(const rocsparse_status& status)
        {
            return status;
        }
        return rocsparse_status_success;
    }
}

/********************************************************************************
 * \brief destroy matrix multiplication descriptor
 *******************************************************************************/
rocsparse_status
    rocsparselt_matmul_alg_selection_destroy(const rocsparselt_matmul_alg_selection algSelection)
{
    if(algSelection == nullptr)
    {
        return rocsparse_status_invalid_pointer;
    }

    // Destruct
    try
    {
        constexpr size_t attrs
            = sizeof(algSelection->attributes) / sizeof(algSelection->attributes[0]);
        for(int i = 0; i < attrs; i++)
        {
            if(algSelection->attributes[i].data_size > 0
               && algSelection->attributes[i].data != nullptr)
                free(algSelection->attributes[i].data);
        }
        delete algSelection;
    }
    catch(const rocsparse_status& status)
    {
        return status;
    }
    return rocsparse_status_success;
}

/********************************************************************************
 * \brief
 *******************************************************************************/
rocsparse_status rocsparselt_matmul_alg_set_attribute(const rocsparselt_handle         handle,
                                                      rocsparselt_matmul_alg_selection algSelection,
                                                      rocsparselt_matmul_alg_attribute attribute,
                                                      const void*                      data,
                                                      size_t                           dataSize)
{
    // Check if algSelection is valid
    if(handle == nullptr)
    {
        return rocsparse_status_invalid_handle;
    }
    else if(data == nullptr || algSelection == nullptr)
    {
        return rocsparse_status_invalid_pointer;
    }
    else if(dataSize <= 0)
    {
        return rocsparse_status_invalid_value;
    }
    else
    {
        // Allocate
        try
        {
            if(algSelection->attributes[attribute].data != nullptr
               && algSelection->attributes[attribute].data_size != dataSize)
            {
                free(algSelection->attributes[attribute].data);
                algSelection->attributes[attribute].data = nullptr;
            }
            if(algSelection->attributes[attribute].data == nullptr)
                algSelection->attributes[attribute].data = malloc(dataSize);
            algSelection->attributes[attribute].data_size = dataSize;
            memcpy(algSelection->attributes[attribute].data, data, dataSize);
            log_trace(handle, "rocsparselt_matmul_alg_set_attribute");
        }
        catch(const rocsparse_status& status)
        {
            return status;
        }
        return rocsparse_status_success;
    }
}

/********************************************************************************
 * \brief
 *******************************************************************************/
rocsparse_status rocsparselt_matmul_alg_get_attribute(const rocsparselt_handle         handle,
                                                      rocsparselt_matmul_alg_selection algSelection,
                                                      rocsparselt_matmul_alg_attribute attribute,
                                                      void*                            data,
                                                      size_t                           dataSize)

{
    // Check if matmulDescr is valid
    if(handle == nullptr)
    {
        return rocsparse_status_invalid_handle;
    }
    else if(data == nullptr || algSelection == nullptr)
    {
        return rocsparse_status_invalid_pointer;
    }
    else if(dataSize <= 0)
    {
        return rocsparse_status_invalid_value;
    }
    else
    {
        try
        {
            if(algSelection->attributes[attribute].data == nullptr)
                return rocsparse_status_invalid_value;
            memcpy(data,
                   algSelection->attributes[attribute].data,
                   std::min(dataSize, algSelection->attributes[attribute].data_size));
            log_trace(handle, "rocsparselt_matmul_alg_get_attribute");
        }
        catch(const rocsparse_status& status)
        {
            return status;
        }
        return rocsparse_status_success;
    }
}

/********************************************************************************
 * \brief
 *******************************************************************************/
rocsparse_status rocsparselt_matmul_plan_init(const rocsparselt_handle               handle,
                                              rocsparselt_matmul_plan*               plan,
                                              const rocsparselt_matmul_descr         matmulDescr,
                                              const rocsparselt_matmul_alg_selection algSelection,
                                              size_t                                 workspaceSize)

{
    // Check if plan is valid
    if(handle == nullptr)
    {
        return rocsparse_status_invalid_handle;
    }
    else if(plan == nullptr || matmulDescr == nullptr || algSelection == nullptr)
    {
        return rocsparse_status_invalid_pointer;
    }
    else
    {
        *plan = nullptr;
        // Allocate
        try
        {
            *plan                   = new _rocsparselt_matmul_plan();
            (*plan)->matmul_descr   = matmulDescr;
            (*plan)->alg_selection  = algSelection;
            (*plan)->workspace_size = workspaceSize;
            log_trace(handle, "rocsparselt_matmul_plan_init");
        }
        catch(const rocsparse_status& status)
        {
            return status;
        }
        return rocsparse_status_success;
    }
}

/********************************************************************************
 * \brief destroy matrix multiplication plan descriptor
 *******************************************************************************/
rocsparse_status rocsparselt_matmul_plan_destroy(const rocsparselt_matmul_plan plan)
{
    if(plan == nullptr)
    {
        return rocsparse_status_invalid_pointer;
    }
    // Destruct
    try
    {
        delete plan;
    }
    catch(const rocsparse_status& status)
    {
        return status;
    }
    return rocsparse_status_success;
}

/********************************************************************************
 * \brief Get rocSPARSELt version
 * version % 100        = patch level
 * version / 100 % 1000 = minor version
 * version / 100000     = major version
 *******************************************************************************/
rocsparse_status rocsparselt_get_version(rocsparselt_handle handle, int* version)
{
    // Check if handle is valid
    if(handle == nullptr)
    {
        return rocsparse_status_invalid_handle;
    }
    *version = ROCSPARSELT_VERSION_MAJOR * 100000 + ROCSPARSELT_VERSION_MINOR * 100
               + ROCSPARSELT_VERSION_PATCH;

    log_trace(handle, "rocsparselt_get_version", *version);

    return rocsparse_status_success;
}

/********************************************************************************
 * \brief Get rocSPARSELt git revision
 *******************************************************************************/
rocsparse_status rocsparselt_get_git_rev(rocsparselt_handle handle, char* rev)
{
    // Check if handle is valid
    if(handle == nullptr)
    {
        return rocsparse_status_invalid_handle;
    }

    if(rev == nullptr)
    {
        return rocsparse_status_invalid_pointer;
    }

    static constexpr char v[] = TO_STR(ROCSPARSELT_VERSION_TWEAK);

    memcpy(rev, v, sizeof(v));

    log_trace(handle, "rocsparselt_get_git_rev", rev);

    return rocsparse_status_success;
}

#ifdef __cplusplus
}
#endif

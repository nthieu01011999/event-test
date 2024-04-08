#ifndef __VVTK_SYSTEM_H__
#define __VVTK_SYSTEM_H__
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Initiates a system reboot.
 *
 * This function is responsible for triggering a system reboot using the VVTK framework.
 * It performs the necessary operations to gracefully restart the system.
 *
 * @return VVTK_RET value indicating the success or failure of the reboot operation.
 *         Use this return value to handle errors or confirm successful reboot.
 */

VVTK_RET vvtk_system_reboot();





#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __VVTK_SYSTEM_H__ */

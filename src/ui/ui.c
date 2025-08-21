#include "ui.h"
#include "ui_driver.h"

void ui_init(void)
{
	lv_init();

	/* Display driver */
	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.flush_cb = ui_driver.flush;
	lv_disp_drv_register(&disp_drv);

	/* Input driver */
	lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);
	indev_drv.type	  = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = ui_driver.read_input;
	lv_indev_drv_register(&indev_drv);

	ui_driver.init();
}

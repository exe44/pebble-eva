#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x63, 0x69, 0x87, 0x66, 0xAB, 0xE7, 0x49, 0xA5, 0x90, 0x21, 0x5F, 0x57, 0xE0, 0x92, 0xC0, 0x6C }
PBL_APP_INFO(MY_UUID,
             "EVA", "exe",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

#define BIG_W 40
#define BIG_H 48
#define SMALL_W 20
#define SMALL_H 24

Window window;

BmpContainer img_morning, img_noon, img_night;

BmpContainer img_number;

BmpContainer img_hr;
BmpContainer img_hr_numbers[11];
BitmapLayer hr_digits[3];

BmpContainer img_min;
BmpContainer img_min_numbers[11];
BitmapLayer min_digits[3];

int current_hr = -1;
int current_min = -1;

int digit_used;
int digit_idxs[3];

void calulate_digit_idxs(int num)
{
  digit_used = 1;

  if (num < 11)
  {
    digit_idxs[0] = num;
  }
  else if (num < 20)
  {
    digit_used = 2;
    digit_idxs[0] = 10;
    digit_idxs[1] = num % 10;
  }
  else
  {
    digit_idxs[0] = num / 10;
    digit_idxs[1] = 10;

    if (num % 10 == 0)
    {
      digit_used = 2;
    }
    else
    {
      digit_used = 3;
      digit_idxs[2] = num % 10;
    }
  }
}

int calculate_12_format(int hr)
{
  if (hr == 0) hr += 12;
  if (hr > 12) hr -= 12;
  return hr;
}

// Called once per minute
void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t)
{
  PblTm currentTime;
  get_time(&currentTime);

  if (current_hr != currentTime.tm_hour)
  {
    current_hr = currentTime.tm_hour;

    if (current_hr >= 5 && current_hr < 12)
    {
      layer_set_hidden(&img_morning.layer.layer, false);
      layer_set_hidden(&img_noon.layer.layer, true);
      layer_set_hidden(&img_night.layer.layer, true);
    }
    else if (current_hr >= 12 && current_hr < 19)
    {
      layer_set_hidden(&img_morning.layer.layer, true);
      layer_set_hidden(&img_noon.layer.layer, false);
      layer_set_hidden(&img_night.layer.layer, true);
    }
    else
    {
      layer_set_hidden(&img_morning.layer.layer, true);
      layer_set_hidden(&img_noon.layer.layer, true);
      layer_set_hidden(&img_night.layer.layer, false);
    }

    calulate_digit_idxs(calculate_12_format(current_hr));

    for (int i = 0; i < 3; ++i)
    {
      if (i < digit_used)
      {
        layer_set_hidden(&hr_digits[i].layer, false);

        bitmap_layer_set_bitmap(&hr_digits[i], &img_hr_numbers[digit_idxs[i]].bmp);
        layer_set_frame(&hr_digits[i].layer, GRect(144 - BIG_W * (2 - i), 0, BIG_W, BIG_H));
        layer_mark_dirty(&hr_digits[i].layer);
      }
      else
      {
        layer_set_hidden(&hr_digits[i].layer, true);
      }
    }

    layer_set_frame(&img_hr.layer.layer, GRect(144 - BIG_W, digit_used > 1 ? BIG_H : 0, BIG_W, BIG_H));
    layer_mark_dirty(&img_hr.layer.layer);
  }

  //

  if (current_min != currentTime.tm_min)
  {
    current_min = currentTime.tm_min;

    calulate_digit_idxs(currentTime.tm_min);

    for (int i = 0; i < 3; ++i)
    {
      if (i < digit_used)
      {
        layer_set_hidden(&min_digits[i].layer, false);

        bitmap_layer_set_bitmap(&min_digits[i], &img_min_numbers[digit_idxs[i]].bmp);
        layer_set_frame(&min_digits[i].layer, GRect(SMALL_W * (1 + i), 168 - SMALL_H, SMALL_W, SMALL_H));
        layer_mark_dirty(&min_digits[i].layer);

      }
      else
      {
        layer_set_hidden(&min_digits[i].layer, true);
      }
    }

    layer_set_frame(&img_min.layer.layer, GRect(SMALL_W * (1 + digit_used), 168 - SMALL_H, SMALL_W, SMALL_H));
    layer_mark_dirty(&img_min.layer.layer);
  }

}

void handle_init(AppContextRef ctx)
{
  (void)ctx;

  window_init(&window, "Eva");
  window_stack_push(&window, true /* Animated */);
  window_set_background_color(&window, GColorBlack);

  resource_init_current_app(&PEBBLE_EVA_RESOURCES);

  //

  bmp_init_container(RESOURCE_ID_MORNING, &img_morning);
  bmp_init_container(RESOURCE_ID_NOON, &img_noon);
  bmp_init_container(RESOURCE_ID_NIGHT, &img_night);
  layer_add_child(&window.layer, &img_morning.layer.layer);
  layer_add_child(&window.layer, &img_noon.layer.layer);
  layer_add_child(&window.layer, &img_night.layer.layer);

  //

  bmp_init_container(RESOURCE_ID_NUMBER_SMALL, &img_number);
  layer_set_frame(&img_number.layer.layer, GRect(0, 168 - SMALL_H, SMALL_W, SMALL_H));
  layer_add_child(&window.layer, &img_number.layer.layer);

  //

  bmp_init_container(RESOURCE_ID_HR_BIG, &img_hr);
  layer_add_child(&window.layer, &img_hr.layer.layer);

  bmp_init_container(RESOURCE_ID_N0_BIG, &img_hr_numbers[0]);
  bmp_init_container(RESOURCE_ID_N1_BIG, &img_hr_numbers[1]);
  bmp_init_container(RESOURCE_ID_N2_BIG, &img_hr_numbers[2]);
  bmp_init_container(RESOURCE_ID_N3_BIG, &img_hr_numbers[3]);
  bmp_init_container(RESOURCE_ID_N4_BIG, &img_hr_numbers[4]);
  bmp_init_container(RESOURCE_ID_N5_BIG, &img_hr_numbers[5]);
  bmp_init_container(RESOURCE_ID_N6_BIG, &img_hr_numbers[6]);
  bmp_init_container(RESOURCE_ID_N7_BIG, &img_hr_numbers[7]);
  bmp_init_container(RESOURCE_ID_N8_BIG, &img_hr_numbers[8]);
  bmp_init_container(RESOURCE_ID_N9_BIG, &img_hr_numbers[9]);
  bmp_init_container(RESOURCE_ID_N10_BIG, &img_hr_numbers[10]);

  for (int i = 0; i < 3; ++i)
  {
    bitmap_layer_init(&hr_digits[i], GRect(0, 0, BIG_W, BIG_H));
    layer_add_child(&window.layer, &hr_digits[i].layer);
  }

  //

  bmp_init_container(RESOURCE_ID_MIN_SMALL, &img_min);
  layer_add_child(&window.layer, &img_min.layer.layer);

  bmp_init_container(RESOURCE_ID_N0_SMALL, &img_min_numbers[0]);
  bmp_init_container(RESOURCE_ID_N1_SMALL, &img_min_numbers[1]);
  bmp_init_container(RESOURCE_ID_N2_SMALL, &img_min_numbers[2]);
  bmp_init_container(RESOURCE_ID_N3_SMALL, &img_min_numbers[3]);
  bmp_init_container(RESOURCE_ID_N4_SMALL, &img_min_numbers[4]);
  bmp_init_container(RESOURCE_ID_N5_SMALL, &img_min_numbers[5]);
  bmp_init_container(RESOURCE_ID_N6_SMALL, &img_min_numbers[6]);
  bmp_init_container(RESOURCE_ID_N7_SMALL, &img_min_numbers[7]);
  bmp_init_container(RESOURCE_ID_N8_SMALL, &img_min_numbers[8]);
  bmp_init_container(RESOURCE_ID_N9_SMALL, &img_min_numbers[9]);
  bmp_init_container(RESOURCE_ID_N10_SMALL, &img_min_numbers[10]);

  for (int i = 0; i < 3; ++i)
  {
    bitmap_layer_init(&min_digits[i], GRect(0, 0, SMALL_W, SMALL_H));
    layer_add_child(&window.layer, &min_digits[i].layer);
  }

  // Ensures time is displayed immediately (will break if NULL tick event accessed).
  // (This is why it's a good idea to have a separate routine to do the update itself.)
  handle_minute_tick(ctx, NULL);
}

void handle_deinit(AppContextRef ctx)
{
  (void)ctx;

  bmp_deinit_container(&img_morning);
  bmp_deinit_container(&img_noon);
  bmp_deinit_container(&img_night);

  bmp_deinit_container(&img_number);

  bmp_deinit_container(&img_hr);
  bmp_deinit_container(&img_min);

  for (int i = 0; i < 11; ++i) {
    bmp_deinit_container(&img_hr_numbers[i]);
    bmp_deinit_container(&img_min_numbers[i]);
  }
}

void pbl_main(void *params)
{
  PebbleAppHandlers handlers = {

    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit,

    // Handle time updates
    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }

  };
  app_event_loop(params, &handlers);
}

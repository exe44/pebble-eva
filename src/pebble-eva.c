#include <pebble.h>

#define BIG_W 40
#define BIG_H 48
#define SMALL_W 20
#define SMALL_H 24

static Window *window;

static GBitmap *img_morning, *img_noon, *img_night;
static BitmapLayer *period_layer;

//

static GBitmap *img_hr_numbers[11];
static BitmapLayer *hr_digits[3];

static GBitmap *img_hr;
static BitmapLayer *hr_layer;

//

static GBitmap *img_min_prefix;
static BitmapLayer *min_prefix_layer;

static GBitmap *img_min_numbers[11];
static BitmapLayer *min_digits[3];

static GBitmap *img_min;
static BitmapLayer *min_layer;

//

static int current_hr = -1;
static int current_min = -1;

static int digit_used;
static int digit_idxs[3];

static void calulate_digit_idxs(int num)
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

static int calculate_12_format(int hr)
{
  if (hr == 0) hr += 12;
  if (hr > 12) hr -= 12;
  return hr;
}

static void handle_minute_tick(struct tm* time, TimeUnits units_changed)
{
  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  Layer* layer;

  if (current_hr != time->tm_hour)
  {
    current_hr = time->tm_hour;

    //

    layer = bitmap_layer_get_layer(period_layer);

    if (current_hr >= 5 && current_hr < 12)
      bitmap_layer_set_bitmap(period_layer, img_morning);
    else if (current_hr >= 12 && current_hr < 19)
      bitmap_layer_set_bitmap(period_layer, img_noon);
    else
      bitmap_layer_set_bitmap(period_layer, img_night);

    layer_mark_dirty(layer);

    //

    calulate_digit_idxs(calculate_12_format(current_hr));

    for (int i = 0; i < 3; ++i)
    {
      layer = bitmap_layer_get_layer(hr_digits[i]);

      if (i < digit_used)
      {
        layer_set_hidden(layer, false);

        bitmap_layer_set_bitmap(hr_digits[i], img_hr_numbers[digit_idxs[i]]);
        layer_set_frame(layer, GRect(bounds.size.w - BIG_W * (2 - i), 0, BIG_W, BIG_H));
        layer_mark_dirty(layer);
      }
      else
      {
        layer_set_hidden(layer, true);
      }
    }

    //

    layer = bitmap_layer_get_layer(hr_layer);
    layer_set_frame(layer, GRect(bounds.size.w - BIG_W, digit_used > 1 ? BIG_H : 0, BIG_W, BIG_H));
    layer_mark_dirty(layer);
  }

  //

  if (current_min != time->tm_min)
  {
    current_min = time->tm_min;

    //

    calulate_digit_idxs(time->tm_min);

    for (int i = 0; i < 3; ++i)
    {
      layer = bitmap_layer_get_layer(min_digits[i]);

      if (i < digit_used)
      {
        layer_set_hidden(layer, false);

        bitmap_layer_set_bitmap(min_digits[i], img_min_numbers[digit_idxs[i]]);
        layer_set_frame(layer, GRect(SMALL_W * (1 + i), bounds.size.h - SMALL_H, SMALL_W, SMALL_H));
        layer_mark_dirty(layer);
      }
      else
      {
        layer_set_hidden(layer, true);
      }
    }

    //

    layer = bitmap_layer_get_layer(min_layer);
    layer_set_frame(layer, GRect(SMALL_W * (1 + digit_used), bounds.size.h - SMALL_H, SMALL_W, SMALL_H));
    layer_mark_dirty(layer);
  }
}

static void window_load(Window* window)
{
  Layer* window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  //

  img_morning = gbitmap_create_with_resource(RESOURCE_ID_MORNING);
  img_noon = gbitmap_create_with_resource(RESOURCE_ID_NOON);
  img_night = gbitmap_create_with_resource(RESOURCE_ID_NIGHT);

  period_layer = bitmap_layer_create(GRect(0, 0, img_morning->bounds.size.w, img_morning->bounds.size.h));
  layer_add_child(window_layer, bitmap_layer_get_layer(period_layer));

  //

  img_hr_numbers[0] = gbitmap_create_with_resource(RESOURCE_ID_N0_BIG);
  img_hr_numbers[1] = gbitmap_create_with_resource(RESOURCE_ID_N1_BIG);
  img_hr_numbers[2] = gbitmap_create_with_resource(RESOURCE_ID_N2_BIG);
  img_hr_numbers[3] = gbitmap_create_with_resource(RESOURCE_ID_N3_BIG);
  img_hr_numbers[4] = gbitmap_create_with_resource(RESOURCE_ID_N4_BIG);
  img_hr_numbers[5] = gbitmap_create_with_resource(RESOURCE_ID_N5_BIG);
  img_hr_numbers[6] = gbitmap_create_with_resource(RESOURCE_ID_N6_BIG);
  img_hr_numbers[7] = gbitmap_create_with_resource(RESOURCE_ID_N7_BIG);
  img_hr_numbers[8] = gbitmap_create_with_resource(RESOURCE_ID_N8_BIG);
  img_hr_numbers[9] = gbitmap_create_with_resource(RESOURCE_ID_N9_BIG);
  img_hr_numbers[10] = gbitmap_create_with_resource(RESOURCE_ID_N10_BIG);

  img_hr = gbitmap_create_with_resource(RESOURCE_ID_HR_BIG);
  hr_layer = bitmap_layer_create(GRect(0, 0, BIG_W, BIG_H));
  bitmap_layer_set_bitmap(hr_layer, img_hr);
  layer_add_child(window_layer, bitmap_layer_get_layer(hr_layer));

  //

  img_min_prefix = gbitmap_create_with_resource(RESOURCE_ID_NUMBER_SMALL);
  min_prefix_layer = bitmap_layer_create(GRect(0, bounds.size.h - SMALL_H, SMALL_W, SMALL_H));
  bitmap_layer_set_bitmap(min_prefix_layer, img_min_prefix);
  layer_add_child(window_layer, bitmap_layer_get_layer(min_prefix_layer));

  img_min_numbers[0] = gbitmap_create_with_resource(RESOURCE_ID_N0_SMALL);
  img_min_numbers[1] = gbitmap_create_with_resource(RESOURCE_ID_N1_SMALL);
  img_min_numbers[2] = gbitmap_create_with_resource(RESOURCE_ID_N2_SMALL);
  img_min_numbers[3] = gbitmap_create_with_resource(RESOURCE_ID_N3_SMALL);
  img_min_numbers[4] = gbitmap_create_with_resource(RESOURCE_ID_N4_SMALL);
  img_min_numbers[5] = gbitmap_create_with_resource(RESOURCE_ID_N5_SMALL);
  img_min_numbers[6] = gbitmap_create_with_resource(RESOURCE_ID_N6_SMALL);
  img_min_numbers[7] = gbitmap_create_with_resource(RESOURCE_ID_N7_SMALL);
  img_min_numbers[8] = gbitmap_create_with_resource(RESOURCE_ID_N8_SMALL);
  img_min_numbers[9] = gbitmap_create_with_resource(RESOURCE_ID_N9_SMALL);
  img_min_numbers[10] = gbitmap_create_with_resource(RESOURCE_ID_N10_SMALL);

  img_min = gbitmap_create_with_resource(RESOURCE_ID_MIN_SMALL);
  min_layer = bitmap_layer_create(GRect(0, 0, SMALL_W, SMALL_H));
  bitmap_layer_set_bitmap(min_layer, img_min);
  layer_add_child(window_layer, bitmap_layer_get_layer(min_layer));

  for (int i = 0; i < 3; ++i)
  {
    hr_digits[i] = bitmap_layer_create(GRect(0, 0, BIG_W, BIG_H));
    layer_add_child(window_layer, bitmap_layer_get_layer(hr_digits[i]));

    min_digits[i] = bitmap_layer_create(GRect(0, 0, SMALL_W, SMALL_H));
    layer_add_child(window_layer, bitmap_layer_get_layer(min_digits[i]));
  }

  // Ensures time is displayed immediately

  time_t timestamp = time(NULL);
  struct tm *time = localtime(&timestamp);
  handle_minute_tick(time, MINUTE_UNIT);
}

static void window_unload(Window *window)
{
  bitmap_layer_destroy(period_layer);
  bitmap_layer_destroy(hr_layer);
  bitmap_layer_destroy(min_prefix_layer);
  bitmap_layer_destroy(min_layer);

  for (int i = 0; i < 3; ++i)
  {
    bitmap_layer_destroy(hr_digits[i]);
    bitmap_layer_destroy(min_digits[i]);
  }

  gbitmap_destroy(img_morning);
  gbitmap_destroy(img_noon);
  gbitmap_destroy(img_night);

  gbitmap_destroy(img_hr);
  gbitmap_destroy(img_min_prefix);
  gbitmap_destroy(img_min);

  for (int i = 0; i <= 10; ++i)
  {
    gbitmap_destroy(img_hr_numbers[i]);
    gbitmap_destroy(img_min_numbers[i]);
  }
}

static void handle_init()
{
  window = window_create();
  window_set_background_color(window, GColorBlack);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void handle_deinit()
{
  window_destroy(window);
}

int main(void)
{
  handle_init();
  app_event_loop();
  handle_deinit();
}

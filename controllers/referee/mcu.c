#include "mcu.h"

WbNodeRef mcu_load_robot(char *robot_file)
{
  WbNodeRef root_node = wb_supervisor_node_get_root();
  WbFieldRef root_children_field = wb_supervisor_node_get_field(root_node, "children");
  char str[100] = "saved_nodes/";
  strcat(str, robot_file);
  wb_supervisor_field_import_mf_node(root_children_field, -1, str);
  return wb_supervisor_field_get_mf_node(root_children_field, -1);
}

void mcu_remove_robot(WbNodeRef robot_node)
{
  wb_supervisor_node_remove(robot_node);
}

WbNodeRef mcu_load_track(char *track_file)
{
  WbNodeRef root_node = wb_supervisor_node_get_root();
  WbFieldRef root_children_field = wb_supervisor_node_get_field(root_node, "children");
  char str[100] = "saved_nodes/";
  strcat(str, track_file);
  wb_supervisor_field_import_mf_node(root_children_field, -1, str);
  return wb_supervisor_field_get_mf_node(root_children_field, -1);
}

void mcu_remove_track(WbNodeRef track_node)
{
  wb_supervisor_node_remove(track_node);
}

int mcu_competitor_nb()
{
  FILE *fp;
  fp = fopen("competition_cars.txt", "r");
  int nb = 0;
  int foo = fscanf(fp, "%d", &nb);
  foo++;
  fclose(fp);
  return nb;
}

void mcu_competitor_list(struct board_data_t *board_data)
{
  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  int nb = mcu_competitor_nb();
  fp = fopen("competition_cars.txt", "r");
  int foo = getline(&line, &len, fp);
  foo++;
  for (int i = 0; i < 3 * nb; i++)
  {
    int foo = getline(&line, &len, fp);
    foo++;
    line[strlen(line) - 1] = 0;
    strcpy(board_data[i].car[0], line);
    foo = getline(&line, &len, fp);
    line[strlen(line) - 1] = 0;
    strcpy(board_data[i].car[1], line);
    foo = getline(&line, &len, fp);
    line[strlen(line) - 1] = 0;
    strcpy(board_data[i].car[2], line);
  }
  fclose(fp);
  for (int i = 0; i < nb; i++)
  {
    for (int j = 0; j < 3; j++)
      board_data[i].points[j] = 0;
  }
}

void mcu_track_list(char list[3][50])
{
  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  fp = fopen("competition_tracks.txt", "r");
  for (int i = 0; i < 3; i++)
  {
    int foo = getline(&line, &len, fp);
    foo++;
    line[strlen(line) - 1] = 0;
    strcpy(list[i], line);
  }
  fclose(fp);
}

void mcu_display_init(WbDeviceTag display)
{
  wb_display_set_font(display, "Lucida Sans Unicode", 20, true);
}

void mcu_display_clear(WbDeviceTag display)
{
  wb_display_set_color(display, 0xE5E5E5);
  wb_display_fill_rectangle(display, 0, 0, wb_display_get_width(display), wb_display_get_height(display));
  wb_display_set_color(display, 0x0000FF);
}

void mcu_display_timings(WbDeviceTag display, long current_lap_time, double *laptimes)
{
  char str[100];
  mcu_display_clear(display);
  wb_display_draw_text(display, "Renesas MCU Rally", 2, 2);
  sprintf(str, "%05.2f", current_lap_time / 1000.0);
  wb_display_draw_text(display, str, 2, 27);
  sprintf(str, "%05.2f", laptimes[0]);
  wb_display_draw_text(display, str, 2, 52);
  sprintf(str, "%05.2f", laptimes[1]);
  wb_display_draw_text(display, str, 2, 77);
  sprintf(str, "%05.2f", laptimes[2]);
  wb_display_draw_text(display, str, 2, 102);
}

WbFieldRef cf_field_soft = NULL;
WbFieldRef cf_field_medium = NULL;
WbFieldRef cf_field_hard = NULL;

void mcu_init_friction(char *name, double value)
{
  WbNodeRef root_node = wb_supervisor_node_get_root();
  WbFieldRef root_children_field = wb_supervisor_node_get_field(root_node, "children");
  WbNodeRef world_node = wb_supervisor_field_get_mf_node(root_children_field, 0);
  WbFieldRef contact_props_field = wb_supervisor_node_get_field(world_node, "contactProperties");
  int contactPropertyCount = wb_supervisor_field_get_count(contact_props_field);
  for (int i = 0; i < contactPropertyCount; i++)
  {
    WbNodeRef contact_props_node = wb_supervisor_field_get_mf_node(contact_props_field, i);
    WbFieldRef name_field = wb_supervisor_node_get_field(contact_props_node, "material2");
    if (strcmp(wb_supervisor_field_get_sf_string(name_field), name) == 0)
    {
      WbFieldRef cf_field = wb_supervisor_node_get_field(contact_props_node, "coulombFriction");
      wb_supervisor_field_set_mf_float(cf_field, 0, value);
      if (strcmp(name, "soft") == 0)
      {
        cf_field_soft = cf_field;
      }
      else if (strcmp(name, "medium") == 0)
      {
        cf_field_medium = cf_field;
      }
      else if (strcmp(name, "hard") == 0)
      {
        cf_field_hard = cf_field;
      }
    }
  }
}

void mcu_set_friction(char *name, double value)
{
  if (strcmp(name, "soft") == 0)
  {
    wb_supervisor_field_set_mf_float(cf_field_soft, 0, value);
  }
  else if (strcmp(name, "medium") == 0)
  {
    wb_supervisor_field_set_mf_float(cf_field_medium, 0, value);
  }
  else if (strcmp(name, "hard") == 0)
  {
    wb_supervisor_field_set_mf_float(cf_field_hard, 0, value);
  }
}

void mcu_camera_mounted()
{
  WbNodeRef root_node = wb_supervisor_node_get_root();
  WbFieldRef root_children_field = wb_supervisor_node_get_field(root_node, "children");
  WbNodeRef viewpoint_node = wb_supervisor_field_get_mf_node(root_children_field, 1);
  WbFieldRef follow_field = wb_supervisor_node_get_field(viewpoint_node, "follow");
  wb_supervisor_field_set_sf_string(follow_field, "robot");
  WbFieldRef follow_type_field = wb_supervisor_node_get_field(viewpoint_node, "followType");
  wb_supervisor_field_set_sf_string(follow_type_field, "Mounted Shot");
  WbFieldRef orientation_field = wb_supervisor_node_get_field(viewpoint_node, "orientation");
  double orientation[4] = {0, -1, 0, 1.5708};
  wb_supervisor_field_set_sf_rotation(orientation_field, orientation);
  WbFieldRef position_field = wb_supervisor_node_get_field(viewpoint_node, "position");
  double position[3] = {-0.5, 0.2, 0};
  wb_supervisor_field_set_sf_vec3f(position_field, position);
}

void mcu_camera_tracking()
{
  WbNodeRef root_node = wb_supervisor_node_get_root();
  WbFieldRef root_children_field = wb_supervisor_node_get_field(root_node, "children");
  WbNodeRef viewpoint_node = wb_supervisor_field_get_mf_node(root_children_field, 1);
  WbFieldRef follow_field = wb_supervisor_node_get_field(viewpoint_node, "follow");
  wb_supervisor_field_set_sf_string(follow_field, "robot");
  WbFieldRef follow_type_field = wb_supervisor_node_get_field(viewpoint_node, "followType");
  wb_supervisor_field_set_sf_string(follow_type_field, "Tracking Shot");
  WbFieldRef orientation_field = wb_supervisor_node_get_field(viewpoint_node, "orientation");
  double orientation[4] = {-1, 0, 0, 1};
  wb_supervisor_field_set_sf_rotation(orientation_field, orientation);
  WbFieldRef position_field = wb_supervisor_node_get_field(viewpoint_node, "position");
  double position[3] = {0, 3, 2};
  wb_supervisor_field_set_sf_vec3f(position_field, position);
}

void mcu_camera_pan_tilt(double waypoint[2])
{
  WbNodeRef root_node = wb_supervisor_node_get_root();
  WbFieldRef root_children_field = wb_supervisor_node_get_field(root_node, "children");
  WbNodeRef viewpoint_node = wb_supervisor_field_get_mf_node(root_children_field, 1);
  WbFieldRef follow_field = wb_supervisor_node_get_field(viewpoint_node, "follow");
  wb_supervisor_field_set_sf_string(follow_field, "robot");
  WbFieldRef follow_type_field = wb_supervisor_node_get_field(viewpoint_node, "followType");
  wb_supervisor_field_set_sf_string(follow_type_field, "Pan and Tilt Shot");
  WbFieldRef position_field = wb_supervisor_node_get_field(viewpoint_node, "position");
  double position[3] = {waypoint[0] + 0.03 * (rand() % 100 - 50), 0.5 + (rand() % 100) / 50.0, waypoint[1] + 0.03 * (rand() % 100 - 50)};
  wb_supervisor_field_set_sf_vec3f(position_field, position);
}

void mcu_leaderboard_display_current(WbNodeRef robot_node, char *data)
{
  char display_string[1000];
  WbFieldRef team_name_field = wb_supervisor_node_get_field(robot_node, "teamName");
  strcpy(display_string, wb_supervisor_field_get_sf_string(team_name_field));
  strcat(display_string, ": ");
  WbFieldRef robot_name_field = wb_supervisor_node_get_field(robot_node, "robotName");
  strcat(display_string, wb_supervisor_field_get_sf_string(robot_name_field));
  wb_supervisor_set_label(0, display_string, 0.002, 0.002, 0.1, 0x000000, 0, "Arial Black");
  wb_supervisor_set_label(1, display_string, 0, 0, 0.1, 0xffffff, 0, "Arial Black");
  wb_supervisor_set_label(2, data, 0.002, 0.852, 0.1, 0x000000, 0, "Arial Black");
  wb_supervisor_set_label(3, data, 0, 0.85, 0.1, 0xffffff, 0, "Arial Black");
}

void mcu_leaderboard_display_leaderboard(struct board_data_t *sorted, int competitor_current, int current_track)
{
  char display_string[2000];
  sprintf(display_string, "Pos.  Team             Robot             Laps  Total time  Best lap  Q.  S.  F.  Total\n");
  for (int i = 0; i < competitor_current; i++)
  {
    char temp_string[100];
    sprintf(temp_string, "%-2d    %-16.16s %-16.16s %5.2f  %10.2f  %8.2f  %2d  %2d  %2d     %2d\n", i + 1, sorted[i].team_name, sorted[i].robot_name, sorted[i].laps[current_track], sorted[i].times[current_track], sorted[i].best_laps[current_track], sorted[i].points[0], sorted[i].points[1], sorted[i].points[2], sorted[i].points[0] + sorted[i].points[1] + sorted[i].points[2]);
    strcat(display_string, temp_string);
  }
  wb_supervisor_set_label(0, "", 0.002, 0.002, 0.1, 0x000000, 0, "Arial Black");
  wb_supervisor_set_label(1, "", 0, 0, 0.1, 0xffffff, 0, "Arial Black");
  wb_supervisor_set_label(2, "", 0.002, 0.852, 0.1, 0x000000, 0, "Arial Black");
  wb_supervisor_set_label(3, "", 0, 0.85, 0.1, 0xffffff, 0, "Arial Black");
  wb_supervisor_set_label(4, display_string, 0.021, 0.101, 0.06, 0x000000, 0, "Lucida Console");
  wb_supervisor_set_label(5, display_string, 0.02, 0.1, 0.06, 0xffffff, 0, "Lucida Console");
}

int compare_qualy(const void *a, const void *b)
{
  struct board_data_t *da = (struct board_data_t *)a;
  struct board_data_t *db = (struct board_data_t *)b;

  if (da->laps[0] == db->laps[0])
  {
    if (da->times[0] == db->times[0])
    {
      if (da->best_laps[0] == db->best_laps[0])
      {
        return 0;
      }
      else if (da->best_laps[0] < db->best_laps[0])
        return -1;
      else
        return 1;
    }
    else if (da->times[0] < db->times[0])
      return -1;
    else
      return 1;
  }
  else if (da->laps[0] < db->laps[0])
    return 1;
  else
    return -1;
}

int compare_reverse(const void *a, const void *b)
{
  struct board_data_t *da = (struct board_data_t *)a;
  struct board_data_t *db = (struct board_data_t *)b;

  if (da->laps[0] == db->laps[0])
  {
    if (da->times[0] == db->times[0])
    {
      if (da->best_laps[0] == db->best_laps[0])
      {
        return 0;
      }
      else if (da->best_laps[0] < db->best_laps[0])
        return 1;
      else
        return -1;
    }
    else if (da->times[0] < db->times[0])
      return 1;
    else
      return -1;
  }
  else if (da->laps[0] < db->laps[0])
    return -1;
  else
    return 1;
}

int compare_sprint(const void *a, const void *b)
{
  struct board_data_t *da = (struct board_data_t *)a;
  struct board_data_t *db = (struct board_data_t *)b;

  if (da->laps[1] == db->laps[1])
  {
    if (da->times[1] == db->times[1])
    {
      if (da->best_laps[1] == db->best_laps[1])
      {
        return 0;
      }
      else if (da->best_laps[1] < db->best_laps[1])
        return -1;
      else
        return 1;
    }
    else if (da->times[1] < db->times[1])
      return -1;
    else
      return 1;
  }
  else if (da->laps[1] < db->laps[1])
    return 1;
  else
    return -1;
}

int compare_feature(const void *a, const void *b)
{
  struct board_data_t *da = (struct board_data_t *)a;
  struct board_data_t *db = (struct board_data_t *)b;

  if (da->laps[2] == db->laps[2])
  {
    if (da->times[2] == db->times[2])
    {
      if (da->best_laps[2] == db->best_laps[2])
      {
        return 0;
      }
      else if (da->best_laps[2] < db->best_laps[2])
        return -1;
      else
        return 1;
    }
    else if (da->times[2] < db->times[2])
      return -1;
    else
      return 1;
  }
  else if (da->laps[2] < db->laps[2])
    return 1;
  else
    return -1;
}

int compare_points(const void *a, const void *b)
{
  struct board_data_t *da = (struct board_data_t *)a;
  struct board_data_t *db = (struct board_data_t *)b;
  if (da->points[0] + da->points[1] + da->points[2] == db->points[0] + db->points[1] + db->points[2])
  {
    if (da->points[2] > db->points[2])
      return 1;
    else if (da->points[2] < db->points[2])
      return -1;
    else
    {
      if (da->points[1] > db->points[1])
        return 1;
      else if (da->points[1] < db->points[1])
        return -1;
      else
      {
        if (da->points[0] > db->points[0])
          return 1;
        else if (da->points[0] < db->points[0])
          return -1;
        else
        {
          return 0;
        }
      }
    }
  }
  else if (da->points[0] + da->points[1] + da->points[2] > db->points[0] + db->points[1] + db->points[2])
    return 1;
  else
    return -1;
}

ssize_t getline(char **lineptr, size_t *n, FILE *stream)
{
  size_t pos;
  int c;

  if (lineptr == NULL || stream == NULL || n == NULL)
  {
    errno = EINVAL;
    return -1;
  }

  c = getc(stream);
  if (c == EOF)
  {
    return -1;
  }

  if (*lineptr == NULL)
  {
    *lineptr = malloc(128);
    if (*lineptr == NULL)
    {
      return -1;
    }
    *n = 128;
  }

  pos = 0;
  while (c != EOF)
  {
    if (pos + 1 >= *n)
    {
      size_t new_size = *n + (*n >> 2);
      if (new_size < 128)
      {
        new_size = 128;
      }
      char *new_ptr = realloc(*lineptr, new_size);
      if (new_ptr == NULL)
      {
        return -1;
      }
      *n = new_size;
      *lineptr = new_ptr;
    }

    ((unsigned char *)(*lineptr))[pos++] = c;
    if (c == '\n')
    {
      break;
    }
    c = getc(stream);
  }

  (*lineptr)[pos] = '\0';
  return pos;
}

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

void mcu_remove_robot()
{
  WbNodeRef root_node = wb_supervisor_node_get_root();
  WbFieldRef root_children_field = wb_supervisor_node_get_field(root_node, "children");
  wb_supervisor_field_remove_mf(root_children_field, -1);
}

int mcu_competitor_nb()
{
  FILE *fp;
  fp = fopen("competition.txt", "r");
  int nb = 0;
  int foo = fscanf(fp, "%d", &nb);
  foo++;
  fclose(fp);
  return nb;
}

void mcu_competitor_list(char list[50][50])
{
  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  int nb = mcu_competitor_nb();
  fp = fopen("competition.txt", "r");
  for (int i = 0; i <= nb; i++)
  {
    int foo = getline(&line, &len, fp);
    foo++;
    if (i > 0)
    {
      line[strlen(line) - 1] = 0;
      strcpy(list[i - 1], line);
    }
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
  double orientation[4] = {-1, 0, 0, 1.5708};
  wb_supervisor_field_set_sf_rotation(orientation_field, orientation);
  WbFieldRef position_field = wb_supervisor_node_get_field(viewpoint_node, "position");
  double position[3] = {0, 4, 0};
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
  wb_supervisor_set_label(0, display_string, 0, 0, 0.1, 0xe9c46a, 0, "Arial Black");
  wb_supervisor_set_label(1, data, 0, 0.90, 0.1, 0xe9c46a, 0, "Arial Black");
}

void mcu_leaderboard_display_leaderboard(int competitor_current, char team_names[50][50], char robot_names[50][50], double laps[50], double times[50], double best_laps[50])
{
  // generate an array to sort
  struct board_data_t tosort[competitor_current];
  for (int i = 0; i < competitor_current; i++)
  {
    strcpy(tosort[i].team_name, team_names[i]);
    strcpy(tosort[i].robot_name, robot_names[i]);
    tosort[i].laps = laps[i];
    tosort[i].total_time = times[i];
    tosort[i].best_lap = best_laps[i];
  }
  qsort(tosort, competitor_current, sizeof tosort[0], compare);

  char display_string[1000];
  sprintf(display_string, "Pos.  Team                             Robot             Laps  Total time  Best lap\n");
  for (int i = 0; i < competitor_current; i++)
  {
    char temp_string[100];
    sprintf(temp_string, "%-2d    %-32.32s %-16.16s %5.2f  %10.2f  %8.2f\n", i + 1, tosort[i].team_name, tosort[i].robot_name, tosort[i].laps, tosort[i].total_time, tosort[i].best_lap);
    strcat(display_string, temp_string);
  }
  wb_supervisor_set_label(2, display_string, 0.02, 0.1, 0.06, 0xe9c46a, 0, "Lucida Console");
}

int compare(const void *a, const void *b)
{
  struct board_data_t *da = (struct board_data_t *)a;
  struct board_data_t *db = (struct board_data_t *)b;

  if (da->laps == db->laps)
  {
    if (da->total_time == db->total_time)
    {
      if (da->best_lap == db->best_lap)
      {
        return 0;
      }
      else if (da->best_lap < db->best_lap)
        return -1;
      else
        return 1;
    }
    else if (da->total_time < db->total_time)
      return -1;
    else
      return 1;
  }
  else if (da->laps < db->laps)
    return 1;
  else
    return -1;
}

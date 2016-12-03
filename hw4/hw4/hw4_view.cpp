#include "hw4.h"
#include "hw4_model.h"
#include "hw4_view.h"
#include "hw4_controller.h"


extern gameDef* g_def;
extern gameState* g_state;
extern int moves_left;

extern const char* color_state1_files[7];
extern const char* color_state0_files[7];

GtkWidget *grid;
GtkWidget *window;
GtkWidget *moves_label;
GtkWidget *score_label;


void activate (GtkApplication *app, gpointer user_data) {
   
   /* create a new window, and set its title */
   window = gtk_application_window_new (app);
   gtk_window_set_title (GTK_WINDOW (window), "Window");
   gtk_window_set_default_size (GTK_WINDOW (window), 40*g_state->get_cols(), 40*g_state->get_rows());
   gtk_container_set_border_width (GTK_CONTAINER (window), 10);

   /* Here we construct the container that is going pack our buttons */
   grid = gtk_grid_new();

   /* Pack the container in the window */
   gtk_container_add (GTK_CONTAINER (window), grid);

   GtkButton *button;
   view_construct_grid();


   //four control buttons + 1 quit button
   button = (GtkButton*) gtk_button_new_with_label (NULL);
   gtk_button_set_image(button, gtk_image_new_from_file ("../images/direction/up.png"));
   g_signal_connect (button, "clicked", G_CALLBACK (swap_up), NULL);
   gtk_grid_attach (GTK_GRID (grid), (GtkWidget*) button, g_state->get_cols(), 0, 1, 1); // need to fix these parameters

   button = (GtkButton*) gtk_button_new_with_label (NULL);
   gtk_button_set_image(button, gtk_image_new_from_file ("../images/direction/down.png"));
   g_signal_connect (button, "clicked", G_CALLBACK (swap_down), NULL);
   gtk_grid_attach (GTK_GRID (grid),(GtkWidget*)  button, g_state->get_cols(), 1, 1, 1);

   button = (GtkButton*) gtk_button_new_with_label (NULL);
   gtk_button_set_image(button, gtk_image_new_from_file ("../images/direction/left.png"));
   g_signal_connect (button, "clicked", G_CALLBACK (swap_left), NULL);
   gtk_grid_attach (GTK_GRID (grid), (GtkWidget*) button, g_state->get_cols(), 2, 1, 1);

   button = (GtkButton*) gtk_button_new_with_label (NULL);
   gtk_button_set_image(button, gtk_image_new_from_file ("../images/direction/right.png"));
   g_signal_connect (button, "clicked", G_CALLBACK (swap_right), NULL);
   gtk_grid_attach (GTK_GRID (grid), (GtkWidget*) button, g_state->get_cols(), 3, 1, 1);

   // button = (GtkButton*) gtk_button_new_with_label ("Quit");
   // g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy), window);
   // gtk_grid_attach (GTK_GRID (grid), (GtkWidget*) button, g_state->get_cols(), 4, 1, 1);


   //generate label string and create label
   
   moves_label = gtk_label_new (NULL);
   view_update_moves_label();
   gtk_grid_attach (GTK_GRID (grid), moves_label, g_state->get_cols(), 4, 1, 1);

   score_label = gtk_label_new(NULL);
   view_update_score_label();
   gtk_grid_attach (GTK_GRID (grid), score_label, g_state->get_cols(), 5, 1, 1);


   gtk_widget_show_all (window);
}

void view_destroy_candy(int row, int col){
  gtk_widget_destroy(gtk_grid_get_child_at(GTK_GRID (grid), row, col));
}

void view_create_candy(int candy, int boardStateNum, int row, int col){
   GtkButton *button = (GtkButton*) gtk_toggle_button_new();
   if (boardStateNum > 0) {
      gtk_button_set_image(button, gtk_image_new_from_file (color_state1_files[candy]));
   } else {
      gtk_button_set_image(button, gtk_image_new_from_file (color_state0_files[candy]));
   }
   gtk_button_set_relief (button, GTK_RELIEF_NONE);
   g_signal_connect (button, "clicked", G_CALLBACK (set_row), (gpointer) row);
   g_signal_connect (button, "clicked", G_CALLBACK (set_col), (gpointer) col);  
   gtk_grid_attach (GTK_GRID (grid), (GtkWidget*) button, col, g_state->get_rows() - 1 - row, 1, 1);   //col,row <--> left offset, top offset

   gtk_widget_show_all (window);
}

void view_destroy_grid() {
   for (int row = 0; row < g_state->get_rows(); row++) {
      for (int column = 0; column < g_state->get_cols(); column++){
         view_destroy_candy(row, column);
      }
   }
}

void view_construct_grid() {

   for (int row = 0; row < g_state->get_rows(); row++) {
      for (int column = 0; column < g_state->get_cols(); column++){
         //reterive element from 2d array
         int color = ((candy*)g_state->get_candy_element(row, column))->get_color(); 
         int boardStateNum = *(int*)g_state->get_boardState_element(row, column);
         view_create_candy(color, boardStateNum, row, column);
      }
   }
}

void view_redraw_grid() {
   view_destroy_grid();
   view_construct_grid();
}


void view_update_moves_label(){
   gchar *label_text;

   int m = g_def->get_movesAllowed() - g_state -> get_movesMade();
   label_text = g_strdup_printf("moves left: %d", m);
   gtk_label_set_text ((GtkLabel*) moves_label, label_text);
   free(label_text);

}

void view_update_score_label(){
   gchar *label_text;
   int s = g_state -> get_currScore();
   label_text = g_strdup_printf("score: %d", s);
   gtk_label_set_text ((GtkLabel*) score_label, label_text);
   free(label_text);
}


int main (int argc, char **argv) {
   if (argc != 2) {
      printf("Missing argument!");
      return 0;
   }
   
   //initialize 
   model_initialize(argv[1]);
   applyTemplate();
   

   //gtk
   GtkApplication *app;
   int status;
   app = gtk_application_new ("candy.crush", G_APPLICATION_FLAGS_NONE);
   g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
   status = g_application_run (G_APPLICATION (app), 0, argv);

   //serialize gamestate
   serialize();


   g_object_unref (app);
   return status;
}

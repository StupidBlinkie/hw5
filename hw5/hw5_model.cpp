//hw4_model

#include "hw4.h"
#include "hw4_model.h"
using namespace std;

//for selected candy
long selected_row;
long selected_col;
int selected_candy_bool = 0;
int moves_left = 30;

gameDef* g_def;
gameState* g_state;

const char* color_state1_files[7] = {"../images/regular/state1/blue.png", "../images/regular/state1/green.png", "../images/regular/state1/orange.png",
                              "../images/regular/state1/purple.png", "../images/regular/state1/red.png", "../images/regular/state1/yellow.png",
                              "../images/regular/state1/nocolor.png"};
const char* color_state0_files[7] = {"../images/regular/state0/blue.png", "../images/regular/state0/green.png", "../images/regular/state0/orange.png",
                              "../images/regular/state0/purple.png", "../images/regular/state0/red.png", "../images/regular/state0/yellow.png",
                              "../images/regular/state0/nocolor.png"};



void model_initialize(char* file){
    g_def = new gameDef();
    g_state = new gameState();

    deserialize(file);
}



/*----------------internal game logic methods for model*---------------*/
bool applyTemplate() {
   bool possibleFire = true;
   int numberTemplatesFired = 0;
   while (possibleFire && numberTemplatesFired < 1000) {
      possibleFire = false;
      possibleFire = (VFour() || HFour() || VThree() || HThree());
      if (possibleFire) {
         numberTemplatesFired++;
         applyGravity();
      }
      
   }
   
   return (numberTemplatesFired > 0);     
}



bool VFour() {
   bool foundFire = false;
   for (int row = 0; row <= g_state->get_rows() - 4; row++) {
      for (int col = 0; col < g_state->get_cols(); col++) {
        foundFire = foundFire || checkVFour(row, col);
         
      }
   }
   return foundFire;
}

bool checkVFour(int row, int col){
   int candy_color = g_state->get_candy_color(row, col);
   if (candy_color == -1) {
      return false;
   }
   for (int i = 1; i<4; i++){
      int curr_color = g_state->get_candy_color(row + i, col);
      if (candy_color != curr_color) {
         return false;
      }
   }
   for (int i = 0; i < 4; i++) {
      g_state->set_candy_color(row + i, col, -1);
      g_state->decrement_boardState_element(row + i, col);
   }
   return true;
}

bool HFour() {
   bool foundFire = false;
   for (int row = 0; row < g_state->get_rows(); row++) {
      for (int col = 0; col <= g_state->get_cols() - 4; col++) {
        foundFire = foundFire || checkHFour(row, col);
      }
   }
   return foundFire;
}

bool checkHFour(int row, int col){
   int candy_color = g_state->get_candy_color(row, col);
   if (candy_color == -1) {
      return false;
   }
   for (int i = 1; i<4; i++){
      int curr_color = g_state->get_candy_color(row, col + i);
      if (candy_color != curr_color) {
         return false;
      }
   }
   for (int i = 0; i < 4; i++) {
      g_state->set_candy_color(row, col + i, -1);
      g_state->decrement_boardState_element(row, col + i);
   }
   return true;
}

bool VThree() {
   bool foundFire = false;
   for (int row = 0; row <= g_state->get_rows() - 3; row++) {
      for (int col = 0; col < g_state->get_cols(); col++) {
        foundFire = foundFire || checkVThree(row, col);
      }
   }
   return foundFire;
}

bool checkVThree(int row, int col){
   int candy_color = g_state->get_candy_color(row, col);
   if (candy_color == -1) {
      return false;
   }
   for (int i = 1; i<3; i++){
      int curr_color = g_state->get_candy_color(row + i, col);
      if (candy_color != curr_color) {
         return false;
      }
   }
   for (int i = 0; i < 3; i++) {
      g_state->set_candy_color(row + i, col, -1);
      g_state->decrement_boardState_element(row + i, col);
   }
   return true;
}

bool HThree() {
   bool foundFire = false;
   for (int row = 0; row < g_state->get_rows(); row++) {
      for (int col = 0; col <= g_state->get_cols() - 3; col++) {
        foundFire = foundFire || checkHThree(row, col);
      }
   }
   return foundFire;
}

bool checkHThree(int row, int col){
   int candy_color = g_state->get_candy_color(row, col);
   if (candy_color == -1) {
      return false;
   }
   for (int i = 1; i<3; i++){
      int curr_color = g_state->get_candy_color(row, col + i);
      if (candy_color != curr_color) {
         return false;
      }
   }
   for (int i = 0; i < 3; i++) {
      g_state->set_candy_color(row, col + i, -1);
      g_state->decrement_boardState_element(row, col + i);
   }
   return true;
}

void applyGravity() {

   
   // apply gravity to all left over candies on board
   for (int col = 0; col < g_state->get_cols(); col++) {
      int lowestFiredRow = 0;
      bool foundFired = false;
      for (int row = 0;  row < g_state->get_rows(); row++) {
         if (g_state->get_candy_color(row, col) == -1 && !foundFired) {
            foundFired = true;
            lowestFiredRow = row;
         } else if (g_state->get_candy_color(row, col) != -1 && foundFired) {
            g_state->swap_candy_elements(row, col, lowestFiredRow, col);
            lowestFiredRow++;
         }
      }
         
   // all -1s should be on top. all left over candies should be on bottom
   // put candies fro extension board in -1 candies;
      //cout << "column:   " << col << endl;
      if (foundFired) {
         for (int row = lowestFiredRow; row < g_state->get_rows(); row++) {
            int offset = g_state->get_extensionOffset(col) % g_def->get_extensionColor_rows();
            //cout << "offset:   " << offset << endl;
            int newColor = *(int*)g_def->get_extensionColor_element(offset, col);
            //cout << "new color:    " << newColor << endl;
            g_state->set_candy_color(row, col, newColor);
            int newOffset = g_state->get_extensionOffset(col) + 1;
            g_state->set_extensionOffset(col, newOffset);
            newOffset = g_state->get_extensionOffset(col) % g_def->get_extensionColor_rows();
            //cout << "new offset:    " << newOffset << endl;
         }
         //cout << endl << endl << endl;
      }
   }
}

//---------------------Model deserialize/serialize functions-------------------------

void deserialize2dArray(json_t *json, bool reading_first_array){
    //read rows
    json_t* json_row = json_object_get(json, "rows");
    int rows = json_integer_value(json_row); 
    //read cols
    json_t* json_col = json_object_get(json, "columns");
    int cols = json_integer_value(json_col);
  
    //read json array
    json_t* json_data = json_object_get(json, "data");

    //store in an temporary int array
    int* data = (int*)malloc(rows * cols *sizeof(int));
        for (size_t i = 0; i < json_array_size(json_data); i++) {
            data[i] = json_integer_value(json_array_get(json_data, i));
    }
    

    //load to different fileds of g_def
    if (reading_first_array){
      g_def->set_extensionColor(rows, cols, data);
    }
    else {
      g_def->set_boardState(rows,cols,data);
    }
    
    //g_def set functions deep copied the data, so free immediately after use
    free(data); 
}

//read json and load into 2d array arr
void deserialize(json_t* json){

    json_t* gamedef_json = json_object_get(json, "gamedef");
    if (gamedef_json == NULL) {
      cout << "did not find gamedef" << endl;
    }

    json_t *extension = json_object_get(gamedef_json, "extensioncolor");
    if (extension == NULL) {
      cout << "did not find extensioncolor" << endl;
    }
    deserialize2dArray(extension, 1); //1 and 0 to indicate if reading first or second array
    //iterate to boardstate field
    json_t *boardstate = json_object_get(gamedef_json, "boardstate");
    deserialize2dArray(boardstate, 0);
    
    //iterate to moves allowed
    int movesAllowed = json_integer_value(json_object_get(gamedef_json, "movesallowed"));

    //iterate to id
    int id = json_integer_value(json_object_get(gamedef_json, "gameid"));
    //iterate to color
    int colors = json_integer_value(json_object_get(gamedef_json, "colors"));

    g_def->set_gameID(id);
    g_def->set_movesAllowed(movesAllowed);
    g_def->set_colors(colors);

    json_t *gamestate_json = json_object_get(json, "gamestate");
    g_state->initialize(g_def, gamestate_json);
    json_decref(json);
}

void serialize(){ 
  json_t* obj = json_object();

  //------------------serialize gamedef--------------------
  json_t* gamedef = json_object();
  //serialize extensioncolor
  json_t* extensioncolor = json_object();
  json_t* json_exten_row = json_integer(g_def->get_extensionColor_rows());
  json_t* json_exten_col = json_integer(g_def->get_extensionColor_cols());
  json_t* json_exten_data = json_array();
  for (int r=0; r < g_def->get_extensionColor_rows(); r++){
    for(int c=0; c < g_def->get_extensionColor_cols(); c++){
      json_t* element =  json_integer(*(int*)g_def->get_extensionColor_element(r, c));
      json_array_append(json_exten_data, element);
      json_decref(element);
    }
  }
  json_object_set(extensioncolor, "rows", json_exten_row);
  json_object_set(extensioncolor, "columns", json_exten_col);
  json_object_set(extensioncolor, "data", json_exten_data);
  
  //serialize boardstate
  json_t* boardstate = json_object();
  json_t* json_board_row = json_integer(g_def->get_boardState_rows());
  json_t* json_board_col = json_integer(g_def->get_boardState_cols());  
  json_t* json_board_data = json_array();
  for (int r=0; r < g_def->get_boardState_rows(); r++){
    for(int c=0; c < g_def->get_boardState_cols(); c++){
      json_t* element =  json_integer(*(int*)g_def->get_boardState_element(r, c));
      json_array_append(json_board_data, element);
      json_decref(element);
    }
  }
  json_object_set(boardstate, "rows", json_board_row);
  json_object_set(boardstate, "columns", json_board_col);
  json_object_set(boardstate, "data", json_board_data);

  json_t* gameid = json_integer(g_def->get_gameID());
  json_t* movesallowed = json_integer(g_def->get_movesAllowed());
  json_t* colors = json_integer(g_def-> get_colors());
  json_object_set(gamedef, "extensioncolor", extensioncolor);
  json_object_set(gamedef, "boardstate", boardstate);
  json_object_set(gamedef, "movesallowed", movesallowed);
  json_object_set(gamedef, "gameid", gameid);
  json_object_set(gamedef, "colors", colors);

  //-------END --- serialize gamedef-----------------------------



  //---------serialize gamestate----------------------------------
  json_t* gamestate = json_object();

  //serialize boardcandies
  json_t* boardcandies = json_object();
  json_t* json_gamestate_row = json_integer(g_state->get_rows());
  json_t* json_gamestate_col = json_integer(g_state->get_cols());  
  json_t* json_boardcandies_data = json_array(); 
  for (int r=0; r < g_state->get_rows(); r++){
    for(int c=0; c < g_state->get_cols(); c++){
      json_t* candy = json_object();
      json_t* candy_color = json_integer(g_state->get_candy_color(r, c));
      json_t* candy_type = json_integer(g_state->get_candy_type(r, c));
      json_object_set(candy, "color", candy_color);
      json_object_set(candy, "type", candy_type);
      //append candy element to data
      json_array_append(json_boardcandies_data, candy);
      json_decref(candy); 
      json_decref(candy_color); 
      json_decref(candy_type); 
    }
  }
  json_object_set(boardcandies, "rows", json_gamestate_row);
  json_object_set(boardcandies, "cols", json_gamestate_col);
  json_object_set(boardcandies, "data", json_boardcandies_data);

  //serialize boardstate
  json_t* gstate_boardstate = json_object(); 
  json_t* json_boardstate_data = json_array();
  for (int r=0; r < g_state->get_rows(); r++){
    for(int c=0; c < g_state->get_cols(); c++){
      json_t* element =  json_integer(*(int*)g_state->get_boardState_element(r, c));
      json_array_append(json_boardstate_data, element);
      json_decref(element);
    }
  }
  json_object_set(gstate_boardstate, "rows", json_gamestate_row);
  json_object_set(gstate_boardstate, "columns", json_gamestate_col);
  json_object_set(gstate_boardstate, "data", json_boardstate_data);

  //serialize extensionoffset
  json_t* extensionoffset = json_array();
  for (int i=0; i < g_state->get_cols(); i++){
      json_t* element =  json_integer(g_state->get_extensionOffset(i));
      json_array_append(extensionoffset, element);
      json_decref(element);
  }
  
  json_t* movesmade = json_integer(g_state->get_movesMade());
  json_t* currentscore = json_integer(g_state->get_currScore());
  json_object_set(gamestate, "boardcandies", boardcandies);
  json_object_set(gamestate, "boardstate", gstate_boardstate);
  json_object_set(gamestate, "movesmade", movesmade);
  json_object_set(gamestate, "currentscore", currentscore);
  json_object_set(gamestate, "extensionoffset", extensionoffset);

  //END ------------------------- serialize gamestate----------------------

  //output 
  json_object_set(obj, "gamedef", gamedef);
  json_object_set(obj, "gamestate", gamestate);
  json_dump_file(obj, "json.out", NULL);

  //decref obj
  json_decref(obj);
  //decref gamedef
  json_decref(gamedef);
  json_decref(extensioncolor);
  json_decref(json_exten_row);
  json_decref(json_exten_col);
  json_decref(json_exten_data);
  json_decref(boardstate);
  json_decref(json_board_row);
  json_decref(json_board_col);
  json_decref(json_board_data);
  json_decref(gameid);
  json_decref(movesallowed);
  json_decref(colors);
  //decref game state
  json_decref(gamestate);
  json_decref(boardcandies);
  json_decref(json_gamestate_row);
  json_decref(json_gamestate_col);
  json_decref(json_boardcandies_data);
  json_decref(gstate_boardstate);
  json_decref(json_boardstate_data);
  json_decref(extensionoffset);
  json_decref(movesmade);
  json_decref(currentscore);

}




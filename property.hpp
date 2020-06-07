#ifndef _PROPERTY_HPP
#define _PROPERTY_HPP
#include <boost/property_tree/json_parser.hpp>
#include <filesystem>
#include <fstream>
#include <string>
namespace pt = boost::property_tree;
namespace fs = std::filesystem;

fs::path json_name = fs::current_path() / fs::u8path("Mine-Sweeper.json");
fs::path log_name  = fs::current_path() / fs::u8path("debug.log");
std::ofstream log_file;

/*   Properties (default)  */
std::string title        = "踩地雷ouo";
std::string gameover_msg = "你被炸死了！";
std::string win_msg      = "你贏了！ ";
std::string info         = "1502 劉亦翔";

bool debug = false;

unsigned int fps = 30;
int side_up      = 50;
int side_down    = 30;
int side_left    = 30;
int side_right   = 30;
int line_width   = 5;
int row          = 10; //橫列
int column       = 10; //直行
int block_size   = 32;
int total_bomb   = 20;
int total_block  = row * column;
int width        = row * (block_size + line_width) + side_left + side_right;
int height       = column * (block_size + line_width) + side_up + side_down;

template <typename T>
void my_get(pt::ptree &tree, const char *name, T &value) {
	try {
		value = tree.get<T>(name);
	} catch (pt::ptree_error &e) {
		log_file << "Error while getting value: \"" << name << "\"" << std::endl;
		return;
	}
}

int load_property() {
	pt::ptree root, tree;
	std::fstream property;
	try {
		if (!fs::exists(fs::status(json_name)) || fs::is_empty(json_name)) {
			if (property.open(json_name, std::ios::out); !property) {
				log_file << "Could not create properties: " << json_name << std::endl;
				return -1;
			} else {
				log_file << "Creating properties: " << json_name << std::endl;
				tree.put("title", title);
				tree.put("gameover_msg", gameover_msg);
				tree.put("win_msg", win_msg);
				tree.put("info", info);

				tree.put("debug", debug);

				tree.put("fps", fps);
				tree.put("side_up", side_up);
				tree.put("side_down", side_down);
				tree.put("side_left", side_left);
				tree.put("side_right", side_right);
				tree.put("line_width", line_width);
				tree.put("row", row);
				tree.put("column", column);
				tree.put("block_size", block_size);
				tree.put("total_bomb", total_bomb);

				root.add_child("Mine-Sweeper", tree);
				pt::write_json(property, root, true);
			}
		} else {
			property.open(json_name, std::ios::in);
			log_file << "Phrasing json file: " << json_name << std::endl;
			pt::read_json(property, root);
			tree = root.get_child("Mine-Sweeper");

			my_get<std::string>(tree, "title", title);
			my_get<std::string>(tree, "gameover_msg", gameover_msg);
			my_get<std::string>(tree, "win_msg", win_msg);
			my_get<std::string>(tree, "info", info);

			my_get<bool>(tree, "debug", debug);

			my_get<unsigned int>(tree, "fps", fps);
			my_get<int>(tree, "side_up", side_up);
			my_get<int>(tree, "side_down", side_down);
			my_get<int>(tree, "side_left", side_left);
			my_get<int>(tree, "side_right", side_right);
			my_get<int>(tree, "line_width", line_width);
			my_get<int>(tree, "row", row);
			my_get<int>(tree, "column", column);
			my_get<int>(tree, "block_size", block_size);
			my_get<int>(tree, "total_bomb", total_bomb);
			total_block = row * column;
			width       = row * (block_size + line_width) + side_left + side_right;
			height      = column * (block_size + line_width) + side_up + side_down;
		}
	} catch (pt::ptree_error &e) {
		log_file << "Exception happened: " << e.what() << std::endl;
		return -1;
	}
	return 0;
}

#endif

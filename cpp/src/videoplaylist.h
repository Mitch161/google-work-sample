#pragma once

#include <map>
#include <string>

#include "flag.h"
#include "video.h"

/** A class used to represent a Playlist */

class VideoPlaylist {
 private:
  std::map<std::string, Video> videos;
  std::string name;

 public:
  VideoPlaylist(const std::string&);
  bool addVideo(const Video*);
  bool erase(const Video*);
  void clear();
  void outputPlaylist(Flag&);
  size_t size();
  std::string getName();
};

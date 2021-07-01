#pragma once

#include <map>
#include <string>

#include "video.h"

class Flag {
 private:
  std::map<std::string, std::string> flaggedVideos;

 public:
  void add(const Video&, const std::string&);
  void add(const Video&);
  void remove(const std::string&);
  bool isExisting(const Video&);
  const std::string getReason(const Video&);
};
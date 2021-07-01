#include "videoplaylist.h"

#include <iostream>

VideoPlaylist::VideoPlaylist(const std::string& name) { this->name = name; }

bool VideoPlaylist::addVideo(const Video* video) {
  bool isSuccess =
      videos.insert(std::make_pair(video->getVideoId(), *video)).second;
  return isSuccess;
}

bool VideoPlaylist::erase(const Video* video) {
  return videos.erase(video->getVideoId());
}

void VideoPlaylist::clear() { videos.clear(); }

void VideoPlaylist::outputPlaylist(Flag& flag) {
  for (auto video = videos.begin(); video != videos.end(); ++video) {
    std::cout << video->second.toString();
    if (flag.isExisting(video->second)) {
      std::cout << " - FLAGGED (reason: " << flag.getReason(video->second)
                << ")";
    }
    std::cout << std::endl;
  }
}

size_t VideoPlaylist::size() { return videos.size(); }

std::string VideoPlaylist::getName() { return name; }
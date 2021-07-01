#include "flag.h"

#include <iostream>

bool Flag::isExisting(const Video& video) {
  auto result = flaggedVideos.find(video.getVideoId());

  if (result != flaggedVideos.end()) {
    return true;
  }

  return false;
}

void Flag::add(const Video& video, const std::string& reason) {
  flaggedVideos.insert(std::make_pair(video.getVideoId(), reason));
}

void Flag::add(const Video& video) {
  flaggedVideos.insert(std::make_pair(video.getVideoId(), "Not supplied"));
}

void Flag::remove(const std::string& videoId) { flaggedVideos.erase(videoId); }

const std::string Flag::getReason(const Video& video) {
  auto result = flaggedVideos.find(video.getVideoId());
  return result->second;
}

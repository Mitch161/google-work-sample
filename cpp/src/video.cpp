#include "video.h"

#include <iostream>
#include <utility>
#include <vector>

Video::Video(std::string&& title, std::string&& videoId,
             std::vector<std::string>&& tags)
    : mTitle(std::move(title)),
      mVideoId(std::move(videoId)),
      mTags(std::move(tags)) {}

const std::string& Video::getTitle() const { return mTitle; }

const std::string& Video::getVideoId() const { return mVideoId; }

const std::vector<std::string>& Video::getTags() const { return mTags; }

const std::string Video::toString() const {
  std::string formattedOutput;
  formattedOutput.append(mTitle);
  formattedOutput.append(" (");
  formattedOutput.append(mVideoId);
  formattedOutput.append(") ");

  formattedOutput.append("[");
  for (size_t tagIndex = 0, tagCount = mTags.size(); tagIndex < tagCount;
       ++tagIndex) {
    formattedOutput.append(mTags.at(tagIndex));

    if (tagIndex != tagCount - 1) {
      formattedOutput.append(" ");
    }
  }
  formattedOutput.append("]");

  return formattedOutput;
}

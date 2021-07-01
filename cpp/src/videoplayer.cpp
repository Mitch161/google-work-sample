#include "videoplayer.h"

#include <algorithm>
#include <iostream>
#include <locale>

#define NON_EXISTING_VIDEO_MSG "Video does not exist"
#define NON_EXISTING_PLAYLIST_MSG "Playlist does not exist"
#define NO_VIDEO_PLAYING_MSG "No video is currently playing"

void displayMenu(std::vector<Video>, const std::string&);
std::string toLowercase(const std::string&);

struct less_than_title {
  inline bool operator()(const Video& videoOne, const Video& videoTwo) {
    return videoOne.getTitle() < videoTwo.getTitle();
  }
};

void VideoPlayer::numberOfVideos() {
  std::cout << mVideoLibrary.getVideos().size() << " videos in the library"
            << std::endl;
}

void VideoPlayer::showAllVideos() {
  std::cout << "Here's a list of all available videos:" << std::endl;

  std::vector<Video> videos = mVideoLibrary.getVideos();

  std::sort(videos.begin(), videos.end(), less_than_title());

  for (size_t index = 0; index < videos.size(); ++index) {
    Video video = videos.at(index);

    std::cout << video.toString();
    if (mFlags.isExisting(video)) {
      std::cout << " - FLAGGED (reason: " << mFlags.getReason(video) << ")";
    }
    std::cout << std::endl;
  }
}

void VideoPlayer::playVideo(const std::string& videoId) {
  const Video* video = mVideoLibrary.getVideo(videoId);

  if (video == nullptr) {
    std::cout << "Cannot play video: " << NON_EXISTING_VIDEO_MSG << std::endl;
    return;
  } else if (mFlags.isExisting(*video)) {
    std::cout << "Cannot play video: Video is currently flagged (reason: "
              << mFlags.getReason(*video) << ")" << std::endl;
    return;
  } else if (currentlyPlayingVideo != nullptr) {
    this->stopVideo();
  }
  currentlyPlayingVideo = video;
  std::cout << "Playing video: " << video->getTitle() << std::endl;
  isPaused = false;
}

void VideoPlayer::stopVideo() {
  if (currentlyPlayingVideo == nullptr) {
    std::cout << "Cannot stop video: " << NO_VIDEO_PLAYING_MSG << std::endl;
    return;
  }

  std::cout << "Stopping video: " << currentlyPlayingVideo->getTitle()
            << std::endl;
  currentlyPlayingVideo = nullptr;
  isPaused = false;
}

void VideoPlayer::playRandomVideo() {
  if (currentlyPlayingVideo != nullptr) {
    this->stopVideo();
  }

  std::vector<Video> videos = mVideoLibrary.getVideos();
  size_t nonFlaggedCount = videos.size();
  for (Video video : mVideoLibrary.getVideos()) {
    if (mFlags.isExisting(video)) {
      --nonFlaggedCount;
    }
  }

  size_t videoCount = videos.size();
  if (videoCount == 0 || nonFlaggedCount == 0) {
    std::cout << "No videos available" << std::endl;
    return;
  }

  srand(time(0));
  int videoIndex = rand() % videoCount;

  this->playVideo(videos.at(videoIndex).getVideoId());
}

void VideoPlayer::pauseVideo() {
  if (isPaused) {
    std::cout << "Video already paused: " << currentlyPlayingVideo->getTitle()
              << std::endl;
    return;
  } else if (currentlyPlayingVideo == nullptr) {
    std::cout << "Cannot pause video: " << NO_VIDEO_PLAYING_MSG << std::endl;
    return;
  }

  std::cout << "Pausing video: " << currentlyPlayingVideo->getTitle()
            << std::endl;
  isPaused = true;
}

void VideoPlayer::continueVideo() {
  if (currentlyPlayingVideo == nullptr) {
    std::cout << "Cannot continue video: " << NO_VIDEO_PLAYING_MSG << std::endl;
    return;
  } else if (!isPaused) {
    std::cout << "Cannot continue video: Video is not paused" << std::endl;
    return;
  }

  std::cout << "Continuing video: " << currentlyPlayingVideo->getTitle()
            << std::endl;
  isPaused = false;
}

void VideoPlayer::showPlaying() {
  if (currentlyPlayingVideo == nullptr) {
    std::cout << "" << NO_VIDEO_PLAYING_MSG << std::endl;
    return;
  }

  std::cout << "Currently playing: " << currentlyPlayingVideo->toString();
  if (isPaused) {
    std::cout << " - PAUSED";
  }
  std::cout << std::endl;
}

void VideoPlayer::createPlaylist(const std::string& playlistName) {
  std::string lowercasePlaylistName = toLowercase(playlistName);

  VideoPlaylist newPlaylist(playlistName);
  const auto result = mVideoPlaylists.insert(
      std::make_pair(std::move(lowercasePlaylistName), std::move(newPlaylist)));

  if (!result.second) {
    std::cout << "Cannot create playlist: A playlist with the same name "
                 "already exists"
              << std::endl;
  } else {
    std::cout << "Successfully created new playlist: " << playlistName
              << std::endl;
  }
}

void VideoPlayer::addVideoToPlaylist(const std::string& playlistName,
                                     const std::string& videoId) {
  auto result = findPlaylist(playlistName);

  if (result == mVideoPlaylists.end()) {
    std::cout << "Cannot add video to " << playlistName << ": "
              << NON_EXISTING_PLAYLIST_MSG << std::endl;
    return;
  }

  const Video* video = mVideoLibrary.getVideo(videoId);

  if (video == nullptr) {
    std::cout << "Cannot add video to " << playlistName << ": "
              << NON_EXISTING_VIDEO_MSG << std::endl;
  } else if (mFlags.isExisting(*video)) {
    std::cout << "Cannot add video to " << playlistName
              << ": Video is currently flagged (reason: "
              << mFlags.getReason(*video) << ")" << std::endl;
  } else if (result->second.addVideo(video)) {
    std::cout << "Added video to " << playlistName << ": "
              << mVideoLibrary.getVideo(videoId)->getTitle() << std::endl;
  } else {
    std::cout << "Cannot add video to " << playlistName
              << ": Video already added" << std::endl;
  }
}

void VideoPlayer::showAllPlaylists() {
  if (mVideoPlaylists.empty()) {
    std::cout << "No playlists exist yet" << std::endl;
    return;
  }

  std::cout << "Showing all playlists:" << std::endl;
  for (auto playlists = mVideoPlaylists.begin();
       playlists != mVideoPlaylists.end(); ++playlists) {
    std::cout << playlists->second.getName() << std::endl;
  }
}

void VideoPlayer::showPlaylist(const std::string& playlistName) {
  auto result = findPlaylist(playlistName);

  if (result == mVideoPlaylists.end()) {
    std::cout << "Cannot show playlist " << playlistName << ": "
              << NON_EXISTING_PLAYLIST_MSG << std::endl;
    return;
  }

  std::cout << "Showing playlist: " << playlistName << std::endl;
  if (result->second.size() == 0) {
    std::cout << "No videos here yet" << std::endl;
    return;
  }

  result->second.outputPlaylist(mFlags);
}

void VideoPlayer::removeFromPlaylist(const std::string& playlistName,
                                     const std::string& videoId) {
  auto result = findPlaylist(playlistName);

  if (result == mVideoPlaylists.end()) {
    std::cout << "Cannot remove video from " << playlistName << ": "
              << NON_EXISTING_PLAYLIST_MSG << std::endl;
    return;
  }

  const Video* video = mVideoLibrary.getVideo(videoId);

  if (video == nullptr) {
    std::cout << "Cannot remove video from " << playlistName << ": "
              << NON_EXISTING_VIDEO_MSG << std::endl;
  } else if (result->second.erase(video)) {
    std::cout << "Removed video from " << playlistName << ": "
              << video->getTitle() << std::endl;
  } else {
    std::cout << "Cannot remove video from " << playlistName
              << ": Video is not in playlist" << std::endl;
  }
}

void VideoPlayer::clearPlaylist(const std::string& playlistName) {
  auto result = findPlaylist(playlistName);

  if (result == mVideoPlaylists.end()) {
    std::cout << "Cannot clear playlist " << playlistName << ": "
              << NON_EXISTING_PLAYLIST_MSG << std::endl;
    return;
  }

  std::cout << "Successfully removed all videos from " << playlistName
            << std::endl;
  result->second.clear();
}

void VideoPlayer::deletePlaylist(const std::string& playlistName) {
  auto result = findPlaylist(playlistName);

  if (result == mVideoPlaylists.end()) {
    std::cout << "Cannot delete playlist " << playlistName << ": "
              << NON_EXISTING_PLAYLIST_MSG << std::endl;
    return;
  }

  std::cout << "Deleted playlist: " << playlistName << std::endl;
  mVideoPlaylists.erase(toLowercase(playlistName));
}

void VideoPlayer::searchVideos(const std::string& searchTerm) {
  std::string lowercaseSearchTerm = toLowercase(searchTerm);
  std::vector<Video> videos = mVideoLibrary.getVideos();

  std::vector<Video> orderedVideos;
  for (Video searchVideo : videos) {
    std::string videoTitle = searchVideo.getTitle();
    std::string lowercaseVideoTitle = toLowercase(videoTitle);

    size_t pos = lowercaseVideoTitle.find(lowercaseSearchTerm);
    if (pos != std::string::npos && !mFlags.isExisting(searchVideo)) {
      orderedVideos.push_back(searchVideo);
    }
  }

  displayMenu(orderedVideos, searchTerm);
}

void VideoPlayer::searchVideosWithTag(const std::string& videoTag) {
  std::string lowercaseVideoTag = toLowercase(videoTag);
  std::vector<Video> videos = mVideoLibrary.getVideos();

  std::vector<Video> orderedVideos;
  for (Video searchVideo : videos) {
    std::vector<std::string> videoTags = searchVideo.getTags();

    for (std::string tag : videoTags) {
      size_t pos = tag.find(lowercaseVideoTag);
      if (pos != std::string::npos && !mFlags.isExisting(searchVideo)) {
        orderedVideos.push_back(searchVideo);
      }
    }
  }

  displayMenu(orderedVideos, videoTag);
}

void VideoPlayer::flagVideo(const std::string& videoId) {
  const Video* video = mVideoLibrary.getVideo(videoId);

  if (video == nullptr) {
    std::cout << "Cannot flag video: " << NON_EXISTING_VIDEO_MSG << std::endl;
    return;
  } else if (currentlyPlayingVideo != nullptr &&
             currentlyPlayingVideo->getVideoId() == video->getVideoId()) {
    this->stopVideo();
  }

  bool result = mFlags.isExisting(*video);

  if (!result) {
    mFlags.add(*video);
    std::cout << "Successfully flagged video: " << video->getTitle()
              << " (reason: Not supplied)" << std::endl;
  } else {
    std::cout << "Cannot flag video: Video is already flagged" << std::endl;
  }
}

void VideoPlayer::flagVideo(const std::string& videoId,
                            const std::string& reason) {
  const Video* video = mVideoLibrary.getVideo(videoId);

  if (video == nullptr) {
    std::cout << "Cannot flag video: " << NON_EXISTING_VIDEO_MSG << std::endl;
    return;
  } else if (currentlyPlayingVideo != nullptr &&
             currentlyPlayingVideo->getVideoId() == video->getVideoId()) {
    this->stopVideo();
  }

  bool result = mFlags.isExisting(*video);

  if (!result) {
    mFlags.add(*video, reason);
    std::cout << "Successfully flagged video: " << video->getTitle()
              << " (reason: " << reason << ")" << std::endl;
  } else {
    std::cout << "Cannot flag video: Video is already flagged" << std::endl;
  }
}

void VideoPlayer::allowVideo(const std::string& videoId) {
  const Video* video = mVideoLibrary.getVideo(videoId);

  if (video == nullptr) {
    std::cout << "Cannot remove flag from video: " << NON_EXISTING_VIDEO_MSG
              << std::endl;
    return;
  } else if (!mFlags.isExisting(*video)) {
    std::cout << "Cannot remove flag from video: Video is not flagged"
              << std::endl;
    return;
  }

  mFlags.remove(video->getVideoId());
  std::cout << "Successfully removed flag from video: " << video->getTitle()
            << std::endl;
}

void VideoPlayer::displayMenu(std::vector<Video> orderedVideos,
                              const std::string& searchTerm) {
  if (orderedVideos.size() == 0) {
    std::cout << "No search results for " << searchTerm << std::endl;
    return;
  }

  std::sort(orderedVideos.begin(), orderedVideos.end(), less_than_title());

  std::cout << "Here are the results for " << searchTerm << ":" << std::endl;

  int position = 1;
  for (Video searchVideo : orderedVideos) {
    std::cout << position << ") " << searchVideo.toString() << std::endl;
    ++position;
  }

  std::cout << "Would you like to play any of the above? If yes, specify the "
               "number of the video."
            << std::endl;
  std::cout << "If your answer is not a valid number, we will assume it's a no."
            << std::endl;
  std::string userInput;
  std::cin >> userInput;

  try {
    int videoNumber = std::stoi(userInput);
    if (videoNumber < 1 || videoNumber >= position) {
      return;
    }

    Video videoChoice = orderedVideos.at(videoNumber - 1);

    playVideo(videoChoice.getVideoId());
  } catch (const std::invalid_argument& _excep) {
  }
}

std::map<std::string, VideoPlaylist>::iterator VideoPlayer::findPlaylist(
    const std::string& playlistName) {
  std::string lowercasePlaylistName = toLowercase(playlistName);
  return mVideoPlaylists.find(lowercasePlaylistName);
}

std::string toLowercase(const std::string& text) {
  std::string lowercase_text = text;
  std::locale loc;
  for (size_t index = 0; index < text.length(); ++index) {
    lowercase_text[index] = std::tolower(lowercase_text[index], loc);
  }

  return lowercase_text;
}
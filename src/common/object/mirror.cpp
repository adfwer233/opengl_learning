#include "common/object/mirror.h"

void Mirror::bind_frame_buffer(int width, int height) {
    glGenFramebuffers(1, &this->frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, this->frame_buffer);

    glGenTextures(1, &this->mirror_texture);
    glBindTexture(GL_TEXTURE_2D, this->mirror_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_INT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->mirror_texture, 0);

    glGenRenderbuffers(1, &this->render_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, this->render_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, this->render_buffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

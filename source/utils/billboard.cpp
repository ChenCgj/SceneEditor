#include <vector>
#include "billboard.h"
#include "texture_manager.h"

using std::vector;

BillBoard::BillBoard() : m_vao{0}, m_vbo{0} {}

void BillBoard::init(const std::initializer_list<float> &pos, const std::string &texture)
{
    if (m_vao == 0) {
        glGenVertexArrays(1, &m_vao);
    }
    glBindVertexArray(m_vao);
    if (m_vbo == 0) {
        glGenBuffers(1, &m_vbo);
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    vector<float> buf(pos.begin(), pos.end());
    glBufferData(GL_ARRAY_BUFFER, sizeof(buf[0]) * buf.size(), &buf[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(buf[0]) * 3, nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    m_textureID = Texture_manager::instance()->load_texture(GL_TEXTURE_2D, texture);
    m_pointCount = buf.size() / 3;
}

bool BillBoard::setProgram(const std::string &vs, const std::string &fs, const std::string &gs)
{
    return m_program.generate_program({vs}, {fs}, {gs});
}

void BillBoard::render(const glm::mat4 &VP, const std::initializer_list<float> &cameraPos)
{
    m_program.use_program();
    m_program.set_uniform("VP", VP);
    m_program.set_uniform("cameraPos", vector<float>(cameraPos.begin(), cameraPos.end()));
    Texture_manager::instance()->bind_texture(m_program, GL_TEXTURE_2D, "tex", m_textureID);
    glBindVertexArray(m_textureID);
    glDrawArrays(GL_POINTS, 0, m_pointCount);
    glBindVertexArray(0);
    Texture_manager::instance()->unbind_texture(m_textureID);
}

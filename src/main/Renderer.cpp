////////////////////////////////////////////////////////////////////////////////////////
//                                                                                    //
//                              Title of the Game                                     //
//                                Renderer.cpp                                        //
//                                Ross Maspero                                        //
//                                                                                    //
////////////////////////////////////////////////////////////////////////////////////////


#include "main/Renderer.hpp"




RenderSystem::RenderSystem(ex::EntityManager &entM) {
   //Generates a VAO for each renderable entity (should also be added to an event create new object)
   entM.each<Renderable, Shader>([this](ex::Entity entity, Renderable &mesh, Shader& pID) {
      genBuffers(entity, mesh, pID);
   });
}


void RenderSystem::update(ex::EntityManager &entM, ex::EventManager &evnM, ex::TimeDelta dT) {

   //Pass the lights to graphics card
   addLight(entM);

   //Find camera as it is required by drawScene this is where you will differentiate between level and menu
   Camera* cam;
   entM.each<Camera>([this, &cam](ex::Entity entity, Camera &camera) {
      cam = &camera;
   });

   //Passes camera and model matrix, and then renders each object
   entM.each<Renderable, Shader>([this, &cam](ex::Entity entity, Renderable &mesh, Shader& pID) {
      drawScene(mesh, pID, cam);
   });

   //Reset the VAO
   glBindVertexArray(0);
}



//This creates the VAOs, which are later used to quick switch between objects to render
void RenderSystem::genBuffers(ex::Entity& ent, Renderable& eVecs, Shader& prog) {

   std::vector<unsigned short> inds;
   std::vector<glm::vec2> uvsInds;
   std::vector<glm::vec3> vertInds, normInds;

   //This performs VBO indexing
   indexVBO(eVecs.verts, eVecs.uvs, eVecs.norms, inds, vertInds, uvsInds, normInds);
   eVecs.indSize = inds.size();

   //Generate and bind VAO
   glGenVertexArrays(1, &eVecs.VAO);
   glBindVertexArray(eVecs.VAO);

   glUseProgram(prog.progID);

   //Bind vertices to shader
   glGenBuffers(1, &eVecs.vertID);
   glBindBuffer(GL_ARRAY_BUFFER, eVecs.vertID);
   glBufferData(GL_ARRAY_BUFFER, vertInds.size() * sizeof(glm::vec3), &vertInds[0], GL_STATIC_DRAW);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

   //Bind texture locations to shader
   glGenBuffers(1, &eVecs.uvID);
   glBindBuffer(GL_ARRAY_BUFFER, eVecs.uvID);
   glBufferData(GL_ARRAY_BUFFER, uvsInds.size() * sizeof(glm::vec2), &uvsInds[0], GL_STATIC_DRAW);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

   //Bind normals to shader
   glGenBuffers(1, &eVecs.normID);
   glBindBuffer(GL_ARRAY_BUFFER, eVecs.normID);
   glBufferData(GL_ARRAY_BUFFER, normInds.size() * sizeof(glm::vec3), &normInds[0], GL_STATIC_DRAW);
   glEnableVertexAttribArray(2);
   glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

   //Bind VBO indices to shader
   glGenBuffers(1, &eVecs.indID);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eVecs.indID);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(unsigned short), &inds[0] , GL_STATIC_DRAW);

   // Bind our texture in Texture Unit 0
   //glUniform1i(glGetUniformLocation(prog.progID, "material.diffuse"), 0);
   //glUniform1i(glGetUniformLocation(prog.progID, "material.specular"), 0);
   glUniform1f(glGetUniformLocation(prog.progID, "material.shininess"), 32.0f);

   //Unbind VAO
   glBindVertexArray(0);

}






void RenderSystem::drawScene(Renderable& mesh, Shader& prog, Camera* cam) {

   //Rebind the objects VAO
   glBindVertexArray(mesh.VAO);

   //Send camera information to the buffer
   glm::mat4 camView = cam->projection * cam->view * mesh.modelMat;
   glUniformMatrix4fv(glGetUniformLocation(prog.progID, "camView"), 1, GL_FALSE, &camView[0][0]);

   //Send object's model matrix
   glUniformMatrix4fv(glGetUniformLocation(prog.progID, "model"), 1, GL_FALSE, &mesh.modelMat[0][0]);

   //Resets the texture and binds correct texture
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(GL_TEXTURE_2D, mesh.texID);

   //Render the object
   glDrawElements(GL_TRIANGLES, mesh.indSize, GL_UNSIGNED_SHORT, (void*)0);

   //Unbind the VAO for the next object
   glBindVertexArray(0);
}





void RenderSystem::addLight(ex::EntityManager& entM) {

   //Runs over all light entities to send the information to the buffer
   GLuint iNum = 0, pID = 0;
   entM.each<Light, Position, Shader>([this, &iNum, &pID]
                                     (ex::Entity entity, Light& l, Position& p, Shader& s) {

      pID = s.progID;

      //Creates a custom name to pass the lights into an array
      std::stringstream bName;
      bName << "light[" << iNum++ << "].";

      glUniform3f(glGetUniformLocation(s.progID, &(bName.str() + "pos")[0]),
                                       p.pos.x, p.pos.y, p.pos.z);

      glUniform3f(glGetUniformLocation(s.progID, &(bName.str() + "ambient")[0]),
                                       l.ambient.x, l.ambient.y, l.ambient.z);

      glUniform3f(glGetUniformLocation(s.progID, &(bName.str() + "diffuse")[0]),
                                       l.diffuse.x, l.diffuse.y, l.diffuse.z);

      glUniform3f(glGetUniformLocation(s.progID, &(bName.str() + "specular")[0]),
                                       l.specular.x, l.specular.y, l.specular.z);

      glUniform1f(glGetUniformLocation(s.progID, &(bName.str() + "linear")[0]), 0.07);
      glUniform1f(glGetUniformLocation(s.progID, &(bName.str() + "quad")[0]), 0.017);
   });

   //Passes the number of lights to the buffer
   glUniform1i(glGetUniformLocation(pID, "lightNum"), iNum);
}

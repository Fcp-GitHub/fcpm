#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

int main()
{
	glm::dvec3 eye{ 0., 3., 2. }, center{ 0., 0., 0. }, up{ 0., 1., 0. };

	auto result{ glm::lookAt(eye, center, up) };
}

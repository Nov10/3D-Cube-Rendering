#include <opencv2/opencv.hpp>
#include "matrix.h"
using namespace cv;

/// <summary>
/// ���� ���� ��ȯ ����� ����մϴ�
/// </summary>
/// <param name="fov">ȭ��(Field Of View)</param>
/// <param name="aspect_ratio">��Ⱦ��</param>
/// <param name="near">�������� �� ������ �� �Ÿ�</param>
/// <param name="far">�������� �� ������ ���� �Ÿ�</param>
/// <returns></returns>
matrix PerspectiveMatrix(double fov, double aspect_ratio, double near, double far) {
	double distance = 0.01;
	double height = tan(fov / 2.0) * distance * 2;;
	double width = height * aspect_ratio;

	//���� ��ȯ ���
	matrix perspective_matrix = matrix4x4(
		2 * near / height,               0,                              0,              0,
		        0,             2 * near / width,                       0,              0,
		        0,      -2 * far * near / (far - near),   -(far + near) / (far - near), 0,
		        0,                      0,                             -1,              0);

	return perspective_matrix;
}
/// <summary>
/// ī�޶��� �� ����� ����մϴ�
/// </summary>
/// <param name="direction">ī�޶��� ����</param>
/// <param name="pos">ī�޶��� ��ġ</param>
/// <param name="up">������ǥ���� ��</param>
/// <returns></returns>
matrix ViewMatrix(vector direction, vector pos, vector up) {
	//ī�޶��� ���� ���ϴ� ����
	vector z_axis = direction / direction.length();
	//ī�޶��� �������� ���ϴ� ����
	vector x_axis = (up.cross(z_axis)) / up.cross(z_axis).length();
	//ī�޶��� ������ ���ϴ� ����
	vector y_axis = z_axis.cross(x_axis);

	//ī�޶��� ȸ�� ���(����)
	//ī�޶��� ��, ������, ������ ���ϴ� ���͸� ��ķ� ���� ��Ÿ���� ȸ�� ����� �˴ϴ�.
	matrix rotation_matrix(3, 3);
	rotation_matrix[0] = x_axis;
	rotation_matrix[1] = y_axis;
	rotation_matrix[2] = -z_axis;  //������ ��ǥ�迡�� z�� -����
	rotation_matrix = rotation_matrix.T();
	
	//������ǥ��� ��ȯ
	rotation_matrix = rotation_matrix.reshape(4, 4, 0);
	rotation_matrix[3][3] = 1;

	//ī�޶��� �̵� ���(����X)
	matrix translation(4, 4);
	//�밢 ������ 1�� ����
	translation[0][0] = translation[1][1] = translation[2][2] = translation[3][3] = 1;
	//ī�޶��� ��ġ�� �̵� ����� ��
	for (int i = 0; i < 3; i++)
		translation[i][3] = -pos[i];

	//ȸ���� ������ �̵��ؾ� �ϹǷ�, ���ϴ� ������ �Ʒ��� ���ƾ� ��
	matrix view_matrix = translation * rotation_matrix;

	return view_matrix;
}

int main() {
	//ȭ�� ���� ũ��
	const int width = 900;
	//ȭ�� ���� ũ��
	const int height = 900;
	Mat image(height, width, CV_8UC3, Scalar(255, 255, 255));
	//��ü�� �����ϴ� ����
	std::list<vector> cube_vertices =
	{
		vector3(2.0, 2.0, 2.0),
		vector3(2.0, 2.0, -2.0),
		vector3(2.0, -2.0, -2.0),
		vector3(2.0, -2.0, 2.0),
		vector3(-2.0, 2.0, 2.0),
		vector3(-2.0, 2.0, -2.0),
		vector3(-2.0, -2.0, -2.0),
		vector3(-2.0, -2.0, 2.0),
	};
	vector* converted_cubes = new vector[cube_vertices.size()];

	//��ü�� �����ϴ� ����
	std::list<std::pair<int, int>> cube_edges = {
	{0, 1}, {1, 2}, {2, 3}, {3, 0},
	{4, 5}, {5, 6}, {6, 7}, {7, 4},
	{0, 4}, {1, 5}, {2, 6}, {3, 7}
	};

	//ī�޶��� ��ġ
	vector camera_position = vector3(0.0, 0.0, 4.0);
	//ī�޶��� ����
	vector camera_rotation = euler(0.0, 0.0, 0.0);
	//ī�޶��� ȭ��
	double fov = 0.1;
	//ȭ�� ��Ⱦ��
	double aspect_ratio = static_cast<double>(width) / height;
	//�������� �� ������ �� �Ÿ�
	double near = 0.1;
	//�������� �� ������ ���� �Ÿ�
	double far = 100.0;
	//������ǥ���� ������ ��Ÿ���� ����, y���� ���� �����մϴ�
	vector world_up = vector3(0, 1, 0);
	//��ü�� ��ġ
	vector object_position = vector3(0.0, 0.0, 0.0);
	//��ü�� ȸ����(���Ϸ�)
	vector object_rotation = vector3(0.0, 0.0, 0.0);
	double time = 0;
	double speed = 1;
	double pi2 = CV_2PI;
	while (true)
	{
		time += 0.01;
		time = std::fmod(time, pi2);

		//������ ���
		image.setTo(Scalar(0, 0, 0));
		//ī�޶��� ������ ��Ÿ���� ���͸� ������ǥ��� ��ȯ
		vector camera_direction = convert_normal2homogeneous(vector3(0, 0, -1));
		//ī�޶� ������ ȸ�� ����� ���
		matrix camera_rotiation_matrix = rotation_from_euler(camera_rotation);
		//���� ī�޶��� ������ ���߾�, ī�޶��� ������ ��Ÿ���� ���͸� ȸ����Ŵ
		vector camera_rotated_position_vector = camera_rotiation_matrix * camera_direction;
		//������ǥ�迡�� �Ϲ� 3���� ��ǥ��� ��ȯ
		vector camera_rotated_direction = convert_homogeneous2normal(camera_rotated_position_vector);
		//�� ��ȯ��� ���
		matrix view = ViewMatrix(camera_rotated_direction, camera_position, world_up);
		//���� ���� ��ȯ ����� ���
		matrix perspective = PerspectiveMatrix(fov, aspect_ratio, near, far);

		//���������� ī�޶��� �������� ��ü�� ��ġ�� ��Ÿ���� ����� ���
		matrix camera_R = perspective * view;
		
		//��ü�� �ð��� ������ ���� ������ �̵���Ŵ
		object_position.e[0] = sin(speed * time);
		object_position.e[1] = cos(speed * time);
		object_position.e[2] = sin(speed * time);
		object_rotation.e[0] = sin(speed * time);
		object_rotation.e[1] = cos(speed * time);
		object_rotation.e[2] = sin(speed * time);

		int counter = 0;
		//���� ������
		for (vector& vertex : cube_vertices) {
			//����(vertex)�� ��ġ�� ������ǥ��� ��ȯ
			vector v_homo = convert_normal2homogeneous(vertex);
			//��ü�� ȸ������ ���߾� ������ ȸ��
			v_homo = rotation_from_euler(object_rotation) * v_homo;
			//��ü�� ��ġ�� ���߾� ������ �̵�
			v_homo = v_homo + convert_normal2homogeneous(object_position);
			//ī�޶��� �������� ���̴� ��ü�� ��ġ�� ���
			v_homo = camera_R * v_homo;
			//������ǥ�迡�� �Ϲ� 3���� ��ǥ��� ��ȯ
			vector transformed_point = convert_homogeneous2normal(v_homo);

			//���� ����
			Point2f img_point(transformed_point[0] / transformed_point[2] + width / 2,
				height / 2 - transformed_point[1] / transformed_point[2]);

			converted_cubes[counter] = vector2(img_point.x, img_point.y);
			std::cout << " [" << counter << "] : " << img_point << "\n";
			cv::circle(image, img_point, 10, Scalar(255, 255, 255), -1);
			counter++;
		}

		//���� ������
		for (const auto& edge : cube_edges) {
			vector pt1 = converted_cubes[edge.first];
			vector pt2 = converted_cubes[edge.second];

			//�� ���� ���� �̾� ���� �׸�
			Point2f img_point1(pt1[0], pt1[1]);
			Point2f img_point2(pt2[0], pt2[1]);

			cv::line(image, img_point1, img_point2, Scalar(255, 255, 255), 10);
		}
		cv::putText(image, "camera position : " + camera_position.print_string(), cv::Point2f(10, 20), 0, 0.5, Scalar(255, 255, 255), 1, 16);
		cv::putText(image, "camera rotation : " + camera_rotation.print_string(), cv::Point2f(300, 20), 0, 0.5, Scalar(255, 255, 255), 1, 16);
		cv::putText(image, "camera View Matrix : ", cv::Point2f(10, 50), 0, 0.5, Scalar(255, 255, 255), 1, 16);
		int l = 0;

		std::istringstream iss(view.print_string());
		std::string line;
		while (std::getline(iss, line, '\n')) {
			cv::putText(image, line, cv::Point2f(190, 50 + l * 20), 0, 0.5, Scalar(255, 255, 255), 1, 16);
			l++;
		}

		cv::putText(image, "camera Projection Matrix : ", cv::Point2f(400, 50), 0, 0.5, Scalar(255, 255, 255), 1, 16);
		iss = std::istringstream(camera_R.print_string());
		l = 0;
		while (std::getline(iss, line, '\n')) {
			cv::putText(image, line, cv::Point2f(400 + 230, 50 + l * 20), 0, 0.5, Scalar(255, 255, 255), 1, 16);
			l++;
		}

		cv::putText(image, "object move speed : " + std::to_string(speed).substr(0, 4), cv::Point2f(10, 140), 0, 0.5, Scalar(255, 255, 255), 1, 16);
		cv::putText(image, "fov(field of view) : " + std::to_string(fov).substr(0, 5), cv::Point2f(10, 160), 0, 0.5, Scalar(255, 255, 255), 1, 16);

		cv::putText(image, "Control Camera Position - WA : Foward/Backward, SD : Right/Left, EQ : Up/Down", cv::Point2f(10, 800), 0, 0.5, Scalar(255, 255, 255), 1, 16);
		cv::putText(image, "Control Camera Rotation - IJ : Rotate in X-axis, KL : Rotate in Y-axis", cv::Point2f(10, 820), 0, 0.5, Scalar(255, 255, 255), 1, 16);
		cv::putText(image, "Control Camera FOV - GH : Focus In/Out", cv::Point2f(10, 840), 0, 0.5, Scalar(255, 255, 255), 1, 16);
		cv::putText(image, "Control Object Move - NM : Speed Up/Down", cv::Point2f(10, 860), 0, 0.5, Scalar(255, 255, 255), 1, 16);
		cv::putText(image, "Reset - R", cv::Point2f(10, 880), 0, 0.5, Scalar(255, 255, 255), 1, 16);
		// �̹��� ���
		imshow("3D Cube Rendering", image);
		int key = waitKey(10);
		if (key == 27)  // ESC Ű�� ������ ����
			break;
		else if (key == 'w')  // 'w' Ű�� ������ ī�޶� ������ �̵�
			camera_position.e[2] -= 0.5;
		else if (key == 's')  // 's' Ű�� ������ ī�޶� �ڷ� �̵�
			camera_position.e[2] += 0.5;
		else if (key == 'a')  // 'a' Ű�� ������ ī�޶� �������� �̵�
			camera_position.e[0] -= 0.5;
		else if (key == 'd')  // 'd' Ű�� ������ ī�޶� ���������� �̵�
			camera_position.e[0] += 0.5;
		else if (key == 'q')  // 'q' Ű�� ������ ī�޶� ���� �̵�
			camera_position.e[1] += 0.5;
		else if (key == 'e')  // 'e' Ű�� ������ ī�޶� �Ʒ��� �̵�
			camera_position.e[1] -= 0.5;

		else if (key == 'i')  // 'i' Ű�� ������ ī�޶� �������� ȸ��
		{
			camera_rotation.e[0] += 0.1;
			camera_rotation.e[0] = fmod(camera_rotation.e[0], pi2);
			if (camera_rotation.e[0] >= pi2)
				camera_rotation.e[0] -= pi2;
		}
		else if (key == 'k')  // 'k' Ű�� ������ ī�޶� �Ʒ������� ȸ��
		{
			camera_rotation.e[0] -= 0.1;
			camera_rotation.e[0] = fmod(camera_rotation.e[0], pi2);
			if (camera_rotation.e[0] < 0)
				camera_rotation.e[0] += pi2;
		}
		else if (key == 'j')  // 'j' Ű�� ������ ī�޶� �������� ȸ��
		{
			camera_rotation.e[1] += 0.1;
			camera_rotation.e[1] = fmod(camera_rotation.e[1], pi2);
		}
		else if (key == 'l')  // 'l' Ű�� ������ ī�޶� ���������� ȸ��
		{
			camera_rotation.e[1] -= 0.1;
			camera_rotation.e[1] = fmod(camera_rotation.e[1], pi2);
		}
		else if (key == 'g')
			fov -= 0.001;
		else if(key == 'h')
			fov += 0.001;
		else if (key == 'r')
		{
			camera_position = vector3(0, 0, 4);
			camera_rotation = vector3(0.0, 0.0, 0.0);
			object_position = vector3(0, 0, 0);
			object_rotation = vector3(0, 0, 0);
		}
		else if (key == 'n')
		{
			speed += 0.1;
			if (speed > 50)
				speed = 50;
		}
		else if (key == 'm')
		{
			speed -= 0.1;
			if (speed <= 0)
				speed = 0;
		}
	}
	return 0;
}

#include <opencv2/opencv.hpp>
#include "matrix.h"
using namespace cv;

/// <summary>
/// 원근 투영 변환 행렬을 계산합니다
/// </summary>
/// <param name="fov">화각(Field Of View)</param>
/// <param name="aspect_ratio">종횡비</param>
/// <param name="near">프러스텀 뷰 볼룸의 끝 거리</param>
/// <param name="far">프러스텀 뷰 볼룸의 시작 거리</param>
/// <returns></returns>
matrix PerspectiveMatrix(double fov, double aspect_ratio, double near, double far) {
	double distance = 0.01;
	double height = tan(fov / 2.0) * distance * 2;;
	double width = height * aspect_ratio;

	//투영 변환 행렬
	matrix perspective_matrix = matrix4x4(
		2 * near / height,               0,                              0,              0,
		        0,             2 * near / width,                       0,              0,
		        0,      -2 * far * near / (far - near),   -(far + near) / (far - near), 0,
		        0,                      0,                             -1,              0);

	return perspective_matrix;
}
/// <summary>
/// 카메라의 뷰 행렬을 계산합니다
/// </summary>
/// <param name="direction">카메라의 방향</param>
/// <param name="pos">카메라의 위치</param>
/// <param name="up">월드좌표계의 위</param>
/// <returns></returns>
matrix ViewMatrix(vector direction, vector pos, vector up) {
	//카메라의 앞을 향하는 벡터
	vector z_axis = direction / direction.length();
	//카메라의 오른쪽을 향하는 벡터
	vector x_axis = (up.cross(z_axis)) / up.cross(z_axis).length();
	//카메라의 위쪽을 향하는 벡터
	vector y_axis = z_axis.cross(x_axis);

	//카메라의 회전 행렬(선형)
	//카메라의 앞, 오른쪽, 위쪽을 향하는 벡터를 행렬로 묶어 나타내면 회전 행렬이 됩니다.
	matrix rotation_matrix(3, 3);
	rotation_matrix[0] = x_axis;
	rotation_matrix[1] = y_axis;
	rotation_matrix[2] = -z_axis;  //오른손 좌표계에선 z가 -방향
	rotation_matrix = rotation_matrix.T();
	
	//동차좌표계로 변환
	rotation_matrix = rotation_matrix.reshape(4, 4, 0);
	rotation_matrix[3][3] = 1;

	//카메라의 이동 행렬(선형X)
	matrix translation(4, 4);
	//대각 성분을 1로 설정
	translation[0][0] = translation[1][1] = translation[2][2] = translation[3][3] = 1;
	//카메라의 위치가 이동 행렬이 됨
	for (int i = 0; i < 3; i++)
		translation[i][3] = -pos[i];

	//회전한 다음에 이동해야 하므로, 곱하는 순서는 아래와 같아야 함
	matrix view_matrix = translation * rotation_matrix;

	return view_matrix;
}

int main() {
	//화면 가로 크기
	const int width = 900;
	//화면 세로 크기
	const int height = 900;
	Mat image(height, width, CV_8UC3, Scalar(255, 255, 255));
	//물체를 구성하는 점들
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

	//물체를 구성하는 선들
	std::list<std::pair<int, int>> cube_edges = {
	{0, 1}, {1, 2}, {2, 3}, {3, 0},
	{4, 5}, {5, 6}, {6, 7}, {7, 4},
	{0, 4}, {1, 5}, {2, 6}, {3, 7}
	};

	//카메라의 위치
	vector camera_position = vector3(0.0, 0.0, 4.0);
	//카메라의 각도
	vector camera_rotation = euler(0.0, 0.0, 0.0);
	//카메라의 화각
	double fov = 0.1;
	//화면 종횡비
	double aspect_ratio = static_cast<double>(width) / height;
	//프러스텀 뷰 볼룸의 끝 거리
	double near = 0.1;
	//프러스텀 뷰 볼룸의 시작 거리
	double far = 100.0;
	//월드좌표계의 위쪽을 나타내는 벡터, y축을 위로 설정합니다
	vector world_up = vector3(0, 1, 0);
	//물체의 위치
	vector object_position = vector3(0.0, 0.0, 0.0);
	//물체의 회전각(오일러)
	vector object_rotation = vector3(0.0, 0.0, 0.0);
	double time = 0;
	double speed = 1;
	double pi2 = CV_2PI;
	while (true)
	{
		time += 0.01;
		time = std::fmod(time, pi2);

		//검은색 배경
		image.setTo(Scalar(0, 0, 0));
		//카메라의 방향을 나타내는 벡터를 동차좌표계로 변환
		vector camera_direction = convert_normal2homogeneous(vector3(0, 0, -1));
		//카메라 각도의 회전 행렬을 계산
		matrix camera_rotiation_matrix = rotation_from_euler(camera_rotation);
		//현재 카메라의 각도에 맞추어, 카메라의 방향을 나타내는 벡터를 회전시킴
		vector camera_rotated_position_vector = camera_rotiation_matrix * camera_direction;
		//동차좌표계에서 일반 3차원 좌표계로 변환
		vector camera_rotated_direction = convert_homogeneous2normal(camera_rotated_position_vector);
		//뷰 변환행렬 계산
		matrix view = ViewMatrix(camera_rotated_direction, camera_position, world_up);
		//원근 투영 변환 행렬을 계산
		matrix perspective = PerspectiveMatrix(fov, aspect_ratio, near, far);

		//최종적으로 카메라의 관점에서 물체의 위치를 나타내는 행렬을 계산
		matrix camera_R = perspective * view;
		
		//물체를 시간이 지남에 따라 적당히 이동시킴
		object_position.e[0] = sin(speed * time);
		object_position.e[1] = cos(speed * time);
		object_position.e[2] = sin(speed * time);
		object_rotation.e[0] = sin(speed * time);
		object_rotation.e[1] = cos(speed * time);
		object_rotation.e[2] = sin(speed * time);

		int counter = 0;
		//점을 렌더링
		for (vector& vertex : cube_vertices) {
			//정점(vertex)의 위치를 동차좌표계로 변환
			vector v_homo = convert_normal2homogeneous(vertex);
			//물체의 회전각에 맞추어 정점을 회전
			v_homo = rotation_from_euler(object_rotation) * v_homo;
			//물체의 위치에 맞추어 정점을 이동
			v_homo = v_homo + convert_normal2homogeneous(object_position);
			//카메라의 관점에서 보이는 물체의 위치를 계산
			v_homo = camera_R * v_homo;
			//동차좌표계에서 일반 3차원 좌표계로 변환
			vector transformed_point = convert_homogeneous2normal(v_homo);

			//직각 투영
			Point2f img_point(transformed_point[0] / transformed_point[2] + width / 2,
				height / 2 - transformed_point[1] / transformed_point[2]);

			converted_cubes[counter] = vector2(img_point.x, img_point.y);
			std::cout << " [" << counter << "] : " << img_point << "\n";
			cv::circle(image, img_point, 10, Scalar(255, 255, 255), -1);
			counter++;
		}

		//선을 렌더링
		for (const auto& edge : cube_edges) {
			vector pt1 = converted_cubes[edge.first];
			vector pt2 = converted_cubes[edge.second];

			//두 개의 점을 이어 선을 그림
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
		// 이미지 출력
		imshow("3D Cube Rendering", image);
		int key = waitKey(10);
		if (key == 27)  // ESC 키를 누르면 종료
			break;
		else if (key == 'w')  // 'w' 키를 누르면 카메라를 앞으로 이동
			camera_position.e[2] -= 0.5;
		else if (key == 's')  // 's' 키를 누르면 카메라를 뒤로 이동
			camera_position.e[2] += 0.5;
		else if (key == 'a')  // 'a' 키를 누르면 카메라를 왼쪽으로 이동
			camera_position.e[0] -= 0.5;
		else if (key == 'd')  // 'd' 키를 누르면 카메라를 오른쪽으로 이동
			camera_position.e[0] += 0.5;
		else if (key == 'q')  // 'q' 키를 누르면 카메라를 위로 이동
			camera_position.e[1] += 0.5;
		else if (key == 'e')  // 'e' 키를 누르면 카메라를 아래로 이동
			camera_position.e[1] -= 0.5;

		else if (key == 'i')  // 'i' 키를 누르면 카메라를 위쪽으로 회전
		{
			camera_rotation.e[0] += 0.1;
			camera_rotation.e[0] = fmod(camera_rotation.e[0], pi2);
			if (camera_rotation.e[0] >= pi2)
				camera_rotation.e[0] -= pi2;
		}
		else if (key == 'k')  // 'k' 키를 누르면 카메라를 아래쪽으로 회전
		{
			camera_rotation.e[0] -= 0.1;
			camera_rotation.e[0] = fmod(camera_rotation.e[0], pi2);
			if (camera_rotation.e[0] < 0)
				camera_rotation.e[0] += pi2;
		}
		else if (key == 'j')  // 'j' 키를 누르면 카메라를 왼쪽으로 회전
		{
			camera_rotation.e[1] += 0.1;
			camera_rotation.e[1] = fmod(camera_rotation.e[1], pi2);
		}
		else if (key == 'l')  // 'l' 키를 누르면 카메라를 오른쪽으로 회전
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

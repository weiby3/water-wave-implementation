#include "packet.h"

/* 随着x变,越靠近boundary,越小，大于boundary后，值为0 */
double decrease_function(double x, double boundary){
    if (x > boundary){
        return 0;
    }
    else {
        return (boundary - x)/boundary;
    }
}

Packet::Packet(){}

Packet::Packet(int x, int y, double force){
    start_time = glfwGetTime();
    last_time = glfwGetTime();
    energy = force;
    printf("this->energy:%f\n", this->energy);
    delay = 0;
    this->x = x;
    this->y = y;
    update_packet();
}

void Packet::update_packet(){

    // /*******************************************
    // 一种波的渲染方式
    // for (int i = 0; i < STRIP_COUNT; i++){
    //     for (int j = 0; j < STRIP_LENGTH; j++){
    //         float packet_range = 5;
    //         float d = sqrt(pow(i-x, 2) + pow(j-y, 2));
    //         //  当d==r的时候取得最大值，自变量是d 
    //         if ( d > packet_range){
    //             point_height[i][j] = 0;
    //         }
    //         else {
    //             point_height[i][j] = 0.2 * sin(PI * d/packet_range);
    //         }
    //         // printf("%f\n", point_height[i][j]);
    //     }
    // }
    // *******************************************/

    //具备能量衰减的渲染方式

    delay = glfwGetTime() - start_time;
    last_time = glfwGetTime();
    double max_delay = 5;

    // 根据波的持续时间，计算当前波的能量大小还有半径
    // 波的能量就是三角函数的A
    // 半径就是后期传播的波的范围的一半


    double cur_r = 0.3 - 0.3 * decrease_function(delay,max_delay); 
    double e_fraction = decrease_function(delay, max_delay);

    double cur_energy = this->energy * e_fraction;

    printf("energy:%f\n", energy);
    printf("delay:%f\n", delay);
    printf("cur_r:%f\n", cur_r);
    printf("cur_energy:%f\n", cur_energy);

    // 根据波的能量大小还有半径，计算当前波的形状
    // cur_energy, cur_r
    for (int i = 0; i < STRIP_COUNT; i++){
        for (int j = 0; j < STRIP_LENGTH; j++){
            float packet_range = DIS_TO_COORDINATE * (2 * cur_r);
            float d = sqrt(pow(i-x, 2) + pow(j-y, 2));
            //  当d==r的时候取得最大值，自变量是d 
            if ( d > packet_range){
                point_height[i][j] = 0;
            }
            else {
                // 计算 0 - packet_range 范围内的点的高度，其他都是0
                // 计算的自变量是d，0-packet_range范围应该是0-XXX-0
                point_height[i][j] = cur_energy * sin(PI * d/packet_range);
            }
            // printf("%f\n", point_height[i][j]);
        }
    }


    // (energy, r, l)

}

float Packet::get_x_y_height(int x, int y){
    // printf("point_height:%f\n", point_height[x][y]);
    return point_height[x][y];
}

PacketManager::PacketManager(){
    clear_height();
    packet_num = 0;
}

/* 将所有波包的高度之和清空为0，方便重新计算 */
void PacketManager::clear_height(){
    for (int i = 0; i < STRIP_COUNT; i++){
        for (int j = 0; j <STRIP_LENGTH; j++){
            point_height[i][j] = 0;
        }
    }
}

/* x, y 指水面的x,y坐标（在水面正上方看下去）， force：波包的力度，0-100 */
void PacketManager::add_packet(int x, int y, double force=50){
    my_packet[packet_num] = Packet(x, y, force);
    packet_num++;
}
/* 每一个时刻都需要更新波包的状态 */
void PacketManager::update_all_packet(){
    for (int i = 0; i < packet_num; i++){
        my_packet[i].update_packet();
    }
}
/* 计算波包高度的总和 */
void PacketManager::calculate_all_packet_height(){
    clear_height();
    for (int i = 0; i < STRIP_COUNT; i++){
        for (int j = 0; j < STRIP_LENGTH; j++){
            for (int k = 0; k < packet_num; k++){
                // printf("get_x_y_h:%f\n", my_packet[k].get_x_y_height(i, j));
                point_height[i][j] += my_packet[k].get_x_y_height(i, j);
            }
            // printf("point_height:%f\n", point_height[i][j]);
        }
    }
}
/* TODO: 每一章都要调用 */
void PacketManager::update_data(){
    update_all_packet();
    calculate_all_packet_height();
}
/* 得到波包总和中，(x,y)对应的波包的高度 */
float PacketManager::get_x_y_height(int x, int y){
    return point_height[x][y];
}

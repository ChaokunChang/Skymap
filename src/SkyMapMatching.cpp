//
// Created by 70700 on 2019/3/3.
//

#include "SkyMapMatching.h"
#include <fstream>
#include <random>

SkyMapMatching::SkyMapMatching(){

    struct timeb time_seed;
    ftime(&time_seed);
    srand(static_cast<unsigned>(time_seed.time*1000) + time_seed.millitm);
}


vector<StarPoint> SkyMapMatching::LoadSky(QString &f_name) {
    QCSVAdapter csv_sky(f_name);
    //int count = 0;
    if(!this->sky_.stars_.empty()) this->sky_.stars_.clear();
    this->sky_.stars_=csv_sky.getRecords();
    this->sky_.count_=this->sky_.stars_.size();
    this->sky_.range_ = {360,180};
    this->sky_.centre_ = StarPoint(-1,180.0,0,0);
    return this->sky_.stars_;
}

void Observation::setProperties(image_properties &prop){
    this->imageWidth = prop.imageWidth;
    this->imageHeight = prop.imageHeight;
    this->imageWidthL = prop.imageWidthL;
    this->imageHeightL = prop.imageHeightL;
    this->focal_length = prop.focal_length;
    this->range_ = {prop.imageWidthL,prop.imageHeightL};
}

vector<StarPoint> SkyMapMatching::LoadImage(QString &f_name,image_properties property) {
    this->SIMULATE = false;
    QCSVAdapter csv_sky(f_name);
    vector<StarPoint> stars = csv_sky.getRecords();
    if(property.focal_length<1e-6) property.focal_length = 28;
    this->image_.setProperties(property);
    double max_x=0,max_y=0;
    for(StarPoint sp:stars){
        max_x = max(max_x,sp.x);
        max_y = max(max_y,sp.y);
    }
    qDebug()<<"Max_x:"<<max_x;
    qDebug()<<"Max_y:"<<max_y;
    qDebug()<<" ImageWidthL:"<<property.imageWidthL;
    qDebug()<<"ImageHeightL:"<<property.imageHeightL;
    for(size_t i=0;i<stars.size();i++){
        stars[i].x *= PIXEL_LENGTH/property.imageWidth;
        stars[i].y *= PIXEL_LENGTH/property.imageHeight;
        //qDebug()<<i<<"th:("<<stars[i].x<<","<<stars[i].y<<")";
    }
    if(!this->image_.stars_.empty()) this->image_.stars_.clear();
    this->image_.stars_ = stars;
    this->image_.count_=this->image_.stars_.size();
    this->image_.RangeStandardization();
    qDebug()<<"image true range: ("<<this->image_.range_.first<<","<<this->image_.range_.second<<")";

    StarPoint center(0,this->image_.range_.first/2,this->image_.range_.second/2,0);
    for(size_t i=0;i<this->image_.count_;i++){
        this->image_.stars_[i].change_coordinate(center);
    }
    return this->image_.stars_;
}

size_t SkyMapMatching::SelectTargetStar() {
    double min_dis = INT32_MAX;
    size_t target = 0;
    for(size_t i=0;i<this->image_.stars_.size();i++){
        StarPoint s = this->image_.stars_[i];
        double dis = pow(s.x,2)+pow(s.y,2);
        if(dis<min_dis){
            min_dis = dis;
            target = i;
        }
    }
    this->__target_star = this->image_.stars_[target];
    return target;
}

void SkyMapMatching::SelectTargetStar(int target) {
    this->__image_target = size_t(target);
    this->__target_star = this->image_.stars_[size_t(target)];
}

void show_angle_distance(const Observation &obv){
    if(obv.count_>10){
        qDebug()<<"Too many stars in image, show 100 random sample of them:";
        for(int i=0;i<100;i++){
            size_t s1,s2=0;
            s1 = random_size_t(0,obv.count_);
            while(s2==s1) s2 = random_size_t(0,obv.count_);
            double angle_distance = getSpotAD(obv.stars_[s1].x,obv.stars_[s1].y,
                                              obv.stars_[s2].x,obv.stars_[s2].y,
                                              obv.focal_length);
            qDebug()<<QString::number(s1)+"--"+QString::number(s2)+" : "
                      +QString::number(angle_distance);

        }
    }
    else {
        for(size_t i=0;i<obv.count_;i++){
            for(size_t j=i+1;j<obv.count_;j++){
                double angle_distance = getSpotAD(obv.stars_[i].x,obv.stars_[i].y,
                                                  obv.stars_[j].x,obv.stars_[j].y,
                                                  obv.focal_length);
                qDebug()<<QString::number(i)+"--"+QString::number(j)+" : "
                          +QString::number(angle_distance);
            }
        }
    }
}

int SkyMapMatching::TriangleModel() {
    if(pTM==nullptr)
    {
        pTM =new TriangleMatching(sky_.stars_.size(), 15.0, 0.02);
        pTM->LoadData(sky_.stars_);
    }
    assert(this->image_.count_>=3);
    qDebug()<<"focal_length:";
    if(this->image_.focal_length>1e-6){
        qDebug()<<this->image_.focal_length;
        qDebug()<<"Belowing are all-possible angle_distance in this image.";
        show_angle_distance(this->image_);
    }else{
        qDebug()<<"There is no focal_length info from this image!";
    }
    qDebug("Threshold:%.2f",pTM->GetThreshold());
    vector<StarPoint> triangle;
    triangle.push_back(this->__target_star);

    double dis12=0.0,dis13=0.0,dis23=0.0;
    int round=0;
    double thresh = pTM->GetThreshold();
    while(round++<100){
        pTM->ChooseAdjacentStars(this->image_.stars_,triangle);
        if(!this->SIMULATE){
            //this->image_.focal_length = 1000;
            qDebug("Info: %d  %d %.2f %.2f",this->image_.imageWidth,this->image_.imageHeight,this->image_.imageWidthL,this->image_.focal_length);
            qDebug("Loc info: %.2f, %.2f, %.2f, %.2f ",triangle[0].x,triangle[0].y,triangle[1].x,triangle[1].y);
            dis12 = getSpotAD(triangle[0].x,triangle[0].y,triangle[1].x,triangle[1].y,this->image_.focal_length);
            dis13 = getSpotAD(triangle[0].x,triangle[0].y,triangle[2].x,triangle[2].y,this->image_.focal_length);
            dis23 = getSpotAD(triangle[1].x,triangle[1].y,triangle[2].x,triangle[2].y,this->image_.focal_length);
        }else {
            dis12 = getSphereAD(triangle[0].x,triangle[0].y,triangle[1].x,triangle[1].y);
            dis13 = getSphereAD(triangle[0].x,triangle[0].y,triangle[2].x,triangle[2].y);
            dis23 = getSphereAD(triangle[1].x,triangle[1].y,triangle[2].x,triangle[2].y);
        }
        if(dis12 < thresh && dis13 < thresh && dis23 < thresh){
            qDebug()<<"dis12    "<<"dis13   "<<"dis23";
            qDebug()<<dis12<<" "<<dis13<<""<<dis23;
            break;
        }
        triangle.pop_back();
        triangle.pop_back();
    }
    if(round>=100){
        qDebug()<<"Trangle Model cannot find proper adjacent stars.";
        return -1;
    }
    pTM->MatchAlgorithm(dis12,dis13,dis23,triangle[0].magnitude,triangle[1].magnitude,triangle[2].magnitude);
    int result_index = pTM->GetCandidate();
    qDebug("Triangle Match Ended with %d!",result_index); //show id.
    return result_index; //编号和index差1
}

int SkyMapMatching::NoOpticModel(){
    if(pNOM==nullptr)
    {
        //NoOpticPara para(15.0,1e-6,0.6,160,80);
        NoOpticPara para(15.0,1e-6,0.6,30,80);
        pNOM = new NoOptic(this->sky_.stars_,para);
    }
    if(NOM_mode==NULL) NOM_mode = 0;
    int result = -1;
    //static vector<size_t> all_redundant;
    if(pNOM->Match(this->__image_target,this->image_.stars_)>0){
        //qDebug("All candidates in NoOptic result: %d",pNOM->CandidateNum);
//        if(pNOM->CandidateNum>1){
//            all_redundant.push_back(pNOM->CandidateNum);
//        }
//        QString output=QString::number(all_redundant.size())+": ";
//        for(size_t i=0;i<all_redundant.size();i++) output+=" "+QString::number(all_redundant.at(i));
//        qDebug()<<output;

        result = pNOM->GetCandidate();
    }
    qDebug()<<"NoOptic Model Ended with "<<result; //show id.
    return result;
}

int SkyMapMatching::RCFIModel(){
    if(pRCFI==nullptr)
    {
        pRCFI = new RCFI(this->sky_.stars_,10,200,this->image_.focal_length);
        //pRCFI->init();
    }
    return this->SIMULATE?pRCFI->efind(this->image_.stars_,this->__target_star):pRCFI->find(this->image_.stars_,this->__target_star);
}

void SkyMapMatching::Match(size_t model) {
    if(!this->candidates_.empty()) this->candidates_.clear();
    switch (model) {
    case 1:{
        int skymap_index2 = TriangleModel();
        if(skymap_index2 >= 0) {
            this->__matching_star = this->sky_.stars_[size_t(skymap_index2)];
            Candidate one("Triangle Model",this->__matching_star);
            this->candidates_.push_back(one);

        }
        else printf("Triangle Model cannot get answer.\n");
        while((skymap_index2=this->pTM->GetCandidate())){
            if(skymap_index2 == -1) break;
            this->__matching_star = this->sky_.stars_[size_t(skymap_index2)];
            Candidate one("Triangle Model",this->__matching_star);
            this->candidates_.push_back(one);
        }
        break;
    }
    case 2:{
        int skymap_index1 = NoOpticModel();
        //NOM_mode = 1;
        if(skymap_index1 >= 0) {
            this->__matching_star = this->sky_.stars_[size_t(skymap_index1)];
            Candidate one("NoOptic Model",this->__matching_star);
            this->candidates_.push_back(one);
        }
        else printf("NoOptic Model cannot get answer.\n");
        size_t maxnum = 0;
        if(NOM_mode==1){
            while(pNOM->CandidateNum>0 && maxnum++<20){
                int id = pNOM->GetCandidate();
                Candidate one("NoOptic Model",this->sky_.stars_[size_t(id)]);
                this->candidates_.push_back(one);
            }
        }
        break;
    }
    case 3:{
        int skymap_index3 = RCFIModel();
        if(skymap_index3 >= 0) {
            this->__matching_star = this->sky_.stars_[size_t(skymap_index3)];
            Candidate one("RCFI Model",this->__matching_star);
            this->candidates_.push_back(one);
        }
        else printf("RCFI Model cannot get answer.\n");
        break;
    }
    default:{
        int skymap_index1 = NoOpticModel();
        if(skymap_index1 >= 0) {
            //printf("NoOptic Model Result: %d\n",skymap_index1);
            this->__matching_star = this->sky_.stars_[size_t(skymap_index1)];
            Candidate one("NoOptic Model",this->__matching_star);
            this->candidates_.push_back(one);
        }
        else printf("NoOptic Model cannot get answer.\n");

        int skymap_index2 = TriangleModel();
        if(skymap_index2 >= 0) {
            //printf("Triangle Model Result: %d \n",skymap_index2);
            this->__matching_star = this->sky_.stars_[size_t(skymap_index2)];
            Candidate one("Triangle Model",this->__matching_star);
            this->candidates_.push_back(one);
        }
        else qDebug("Triangle Model cannot get answer.\n");

        int skymap_index3 = RCFIModel();
        if(skymap_index3 >= 0) {
            this->__matching_star = this->sky_.stars_[size_t(skymap_index3)];
            Candidate one("RCFI Model",this->__matching_star);
            this->candidates_.push_back(one);
        }
        else printf("RCFI Model cannot get answer.\n");

        if(skymap_index1<0 && skymap_index2<0 && skymap_index3<0) {
            qDebug("No Model get answer.\n");
        }
    }
    }
}

bool star_point_cmp(const StarPoint &s1, const StarPoint &s2){
    double dis1 = sqrt(pow(s1.x,2)+pow(s1.y,2));
    double dis2 = sqrt(pow(s2.x,2)+pow(s2.y,2));
    return dis1<dis2;
}

bool similar_position(StarPoint &s1, StarPoint &s2){
    return abs(s1.Distance(s2)) <0.001;
}

bool similar_vector(vector<StarPoint> &vec1, vector<StarPoint> &vec2){
    /*TODO:
     * compare two vector,
     * without consideration of the absolute position, but the relative position.*/
    return true;
}

int SkyMapMatching::Check() {
    if(this->SIMULATE){
        return this->__target_star.index == this->__matching_star.index ? 1:-1;
    }
    //get the __matching_star's offset in check_set, which should be equal to offset of __target_star in sky_image.
    StarPoint check_center = this->__matching_star;
    check_center.x -= this->__target_star.x;
    check_center.y -= this->__target_star.y;
    vector<StarPoint> check_set = this->sky_.Subset(check_center,this->image_.range_.first,this->image_.range_.second);
//    for(size_t i=0;i<check_set.size();i++){
//        check_set[i].change_coordinate(check_center);
////        check_set[i].change_coordinate(this->__matching_star);
//    }
    sort(check_set.begin(),check_set.end(),star_point_cmp);

    vector<StarPoint> candidate_set(this->image_.stars_);
//    for(size_t i=0;i<candidate_set.size();i++){
//        candidate_set[i].change_coordinate(this->__target_star);
//    }
    sort(candidate_set.begin(),candidate_set.end(),star_point_cmp);

    size_t len = min(check_set.size(),candidate_set.size());
    double loss = 0;
    size_t i=0,j=0;
    QString str="";
    while(i<check_set.size() && j<candidate_set.size()){
        if(similar_position(check_set[i],candidate_set[j])){
            loss += abs(check_set[i].Distance(candidate_set[j]));
            i++;j++;
        }else if(check_set[i].Module() < candidate_set[j].Module()){
            loss += 1;
            i++;
        } else{
            loss += 1;
            j++;
        }
        str+=QString::number(loss)+" ";
    }
    //qDebug()<<str;
    qDebug()<<"Finale Loss:"<<loss;
    if(loss/len > 0.5) return -1;
    return this->__matching_star.index;
}

int SkyMapMatching::CheckAllCandidates(){
    if(this->candidates_.empty()) return -1;
    int passnum = 0;
    int re_index = -1;
    for(const Candidate& one:this->candidates_){
        StarPoint sp = one.star;
        StarPoint cache = this->__matching_star;
        this->__matching_star = sp;
        qDebug()<<"Checking answer from"<<QString::fromStdString(one.model_name)<<"::"<<sp.index<<" ("<<sp.x<<","<<sp.y<<") "<<sp.magnitude;
        int pass = Check();
        if(pass == -1){
            qDebug()<<"This candidate didn't pass checking";
            this->__matching_star = cache;
        }else{
            qDebug()<<"Answer from"<<QString::fromStdString(one.model_name)<<"Passed!";
            re_index = sp.index;
            passnum++;
        }
    }
    qDebug()<<"total passed model:"<<passnum;
    return re_index;
}

vector<StarPoint> SkyMap::Subset(const StarPoint &centre, double length, double width) {
    //Some Check here.
    vector<StarPoint> stars;
    for(StarPoint point : stars_){
        if(point.InRange(centre,length,width)){
            stars.push_back(point);
        }
    }
    return stars;

}

void SkyMapMatching::GenerateSimImage(const StarPoint &centre, const double &length, const double &width) {
    //Some Check here.
    qDebug()<<"@Generating SimImage...";
    if(length<=0 || width<=0 ){
        if(!this->image_.stars_.empty()) this->image_.stars_.clear();
        this->image_.count_ = 0;
        this->image_.range_ = {length<0?0:length , width<0?0:width};
        return;
    }
    if(!this->image_.stars_.empty()) this->image_.stars_.clear();
    this->image_.stars_=this->sky_.Subset(centre,length,width);
//    for (size_t i=0;i!=this->image_.stars_.size();i++) {
//        this->image_.stars_[i].change_coordinate(centre);
//    }

    this->image_.count_ = this->image_.stars_.size();
    this->image_.centre_ = centre;
    image_properties prop(0,0,length,width,0.0);
    this->image_.setProperties(prop);

    qDebug()<<"The Generated image is:";
    qDebug()<<"*center: "<<centre.index<<"-> ("<<centre.x<<" , "<<centre.y<<")";
    qDebug()<<"*range : "<<length<<" * "<<width;
    qDebug()<<"Image Size:"<<this->image_.stars_.capacity();
}

vector<StarPoint> SkyMap::Subset(const StarPoint &centre, double image_ratio, int numi=0) {
    double upper_bound = 20;
    double lower_bound = 1;
    double length = (upper_bound+lower_bound)/2;
    vector<StarPoint> stars;
    double width;
    size_t num = size_t(numi);
    while( abs(lower_bound-upper_bound)>0.01 ){
        width = length/image_ratio;
        stars = this->Subset(centre,length,width);
        if(stars.size() == num) break;
        else if(stars.size() < num){
            stars.clear();
            lower_bound = length;
            length = (upper_bound+lower_bound)/2;
        } else{
            stars.clear();
            upper_bound = length;
            length = (upper_bound+lower_bound)/2;
        }
    }
    if(stars.size() == num) {
        qDebug()<<"The number of star of this Subset:"<<stars.size();
    } else{
        qDebug()<<"Invalid subset! You can try another ratio or center.";
        stars.clear();
    }
    return stars;
}

void SkyMapMatching::GenerateSimImage(const StarPoint &centre, const double &image_ratio, const int &num) {
    vector<StarPoint> stars = sky_.Subset(centre,image_ratio,num);
    if(!this->image_.stars_.empty()) this->image_.stars_.clear();
    if(stars.size() == size_t(num)) {
//        for(size_t i=0;i<stars.size();i++){
//            stars[i].change_coordinate(centre);
//        }
        this->image_.stars_.insert(this->image_.stars_.end(),stars.begin(),stars.end());
        this->image_.count_ = this->image_.stars_.size();
        this->image_.centre_ = centre;
        qDebug()<<"The number of stars is:"<<stars.size();
    } else{
        this->image_.count_ = 0;
        this->image_.centre_ = centre;
        qDebug()<<"Invalid Subset! You can try another ratio or center.";
    }
    this->image_.RangeStandardization();
    this->image_.imageWidthL = this->image_.range_.first;
    this->image_.imageHeightL = this->image_.range_.second;
}

void Observation::RangeStandardization(){
    if(this->count_==0){
        this->range_ = {0,0};
        return;
    }else{
        double lx = 10000.0,rx = -10000.0,ly = 10000.0,ry = -10000.0;
        for(size_t i=0;i<stars_.size();i++){
            lx = min(stars_[i].x,lx);
            rx = max(stars_[i].x,rx);
            ly = min(stars_[i].x,ly);
            ry = max(stars_[i].x,ry);
        }
        this->range_ = {rx-lx,ry-ly};
    }
}

StarPoint random_point(double l,double r,double u,double d){
    double x = random_double(l,r);
    double y = random_double(d,u);
    StarPoint sp(-1,x,y,-1);
    return sp;
}

size_t central_star(vector<StarPoint> &stars){
    double min_dis = INT32_MAX;
    size_t target = 0;
    for(size_t i=0;i<stars.size();i++){
        StarPoint s = stars[i];
        double dis = pow(s.x,2)+pow(s.y,2);
        if(dis<min_dis){
            min_dis = dis;
            target = i;
        }
    }
    return target;
}

void RandomMissing(vector<StarPoint> &stars, size_t miss_num){
    assert(miss_num>=0 && miss_num<stars.size());
    if(miss_num==0) return;
    vector<size_t> missed(miss_num);
    //for(size_t i=0;i<miss_num;i++) missed[i]=i;
    shuffle(stars.begin(),stars.end(),std::mt19937(std::random_device()()));
    while(miss_num-->0) stars.pop_back();
}

void Observation::ContentSync(){
    this->count_ = this->stars_.size();
}

void RandomAddPoints(vector<StarPoint> &stars,double length, double width, size_t redundence_num){
    assert(redundence_num>=0);
    if(redundence_num==0) return;
    while(redundence_num-->0){
        StarPoint sp = random_point(-length/2,length/2,-width/2,width/2);
        stars.push_back(sp);
    }
}

void RandomDiviation(vector<StarPoint> &stars, double off_rate, size_t type=0){
    assert(off_rate>=0);
    if(abs(off_rate)<=1e-9) return;
    if(type==1){
        //diffusing from center...(the direction is definate,but offset is random(less or equal than off_rate).)
        for(size_t i=0;i<stars.size();i++){
            double randoff = random_double(0.0,off_rate);
            stars[i].x *= (1+randoff);
            stars[i].y *= (1+randoff);
        }
    }
    else if(type==2){
        //diffusing from center...(the direction and offset is definate.)
        for(size_t i=0;i<stars.size();i++){
            stars[i].x *= (1+off_rate);
            stars[i].y *= (1+off_rate);
        }
    }
    else{
        //total random,random offset and random direction.
        for(size_t i=0;i<stars.size();i++){
            double randoffx = random_double(0.0,off_rate);
            stars[i].x *= (1+randoffx);
            double randoffy = random_double(0.0,off_rate);
            stars[i].y *= (1+randoffy);
        }
    }
}

ModelEvaluation SkyMapMatching::ComprehensiveEvaluation(size_t model, size_t round, size_t miss_num,
                                                        size_t add_num, double offset_rate){
    //comprehensive test.
    qDebug()<<endl<<endl<<endl;
    qDebug()<<"Evaluation start....";
    qDebug()<<"Total round number:"<<round;
    assert(round>0);
    int succeed_num=0;
    int failed_num = 0;
    size_t r=0;
    int counts=0;
    StarPoint center;
    double fl=15.0,fr=15.0;
    while(r<round){
        if(r==63){
            cout<<""<<endl;
        }
        qDebug()<<"--------------------------------------------------------------";
        qDebug()<<"-----------------------start:"<<r+1<<"th---------------------------";
        center = random_point(0.0,this->LongitudeRange,- this->LatitudeRange/2,this->LatitudeRange/2);
        this->GenerateSimImage(center,fl,fr);
        if(this->image_.count_>=3){
            //add noise....
            qDebug()<<"@Adding noise...";
            if(miss_num>=this->image_.count_){
                qDebug()<<"There is no enough stars to be deleted...";
                continue;
            }
            RandomMissing(this->image_.stars_,miss_num);
            this->image_.count_ -= miss_num;

            RandomAddPoints(this->image_.stars_,this->image_.imageWidthL,this->image_.imageHeightL,add_num);
            this->image_.count_ += add_num;
            if(this->image_.count_<3){
                qDebug()<<"Impossible after adding noise.";
                continue;
            }
            RandomDiviation(this->image_.stars_,offset_rate);

            this->__image_target=this->SelectTargetStar();
            qDebug()<<"The Target star(skymap's index):<--"<<this->__target_star.index<<" "<<this->__target_star.x<<','<<this->__target_star.y<<" -->";
            qDebug()<<"@Matching...";
            this->Match(model);
            qDebug()<<"@Checking...";
            if(this->CheckAllCandidates() == -1){
                failed_num ++;
                qDebug("Failed the %dth round.\n",r+1);
            }
            else {
                succeed_num ++;
                qDebug("Passed the %dth round.\n",r+1);
            }
            qDebug()<<"-----------------------end:"<<r+1<<"th---------------------------";
            r++;
        }
        else {qDebug()<<"-----------------------retry:"<<r+1<<"th---------------------------";counts++;}
        qDebug()<<"--------------------------------------------------------------";
        qDebug("Retry Counts:%d",counts);
    }
    double ans = (succeed_num)/(succeed_num+failed_num);
    ModelEvaluation eval(succeed_num+failed_num,ans,"Triangle Model");
    return eval;
}

ModelEvaluation SkyMapMatching::ExeSimulation(size_t model,size_t round,size_t miss_num,
                                              size_t add_num,double off_rate){
    this->SIMULATE = true;
    string output;
    if(model == 2){
        size_t sround = round;
        round = 300;
        output="All result:\n";
//        for(size_t i=0;i<10;i++){
//            qDebug("$$$$$$$$$$$$$$$$%d th: LowerAdjacent: %.2f",i+1,0.1*i);
//            delete pNOM;
//            pNOM=nullptr;
//            double la = 0.1*i;
//            NoOpticPara para(15.0,1e-6,  la  ,35,80);
//            pNOM = new NoOptic(this->sky_.stars_,para);
//            ModelEvaluation eval1=this->ComprehensiveEvaluation(model,round,miss_num,add_num,off_rate);
//            output += "LowerAdjacent=" + to_string(la) +":"+ to_string(eval1.accuracy)+"\n";
//            cout<<output<<endl;
//            cout.flush();
//        }

//        for(size_t i=11;i<=20;i++){
//            for(size_t j=1;j<=10;j++){
//                delete pNOM;
//                pNOM=nullptr;
//                size_t cp=i*10,rp=j*10;
//                NoOpticPara para(15.0,1e-6,0.5,rp,cp);
//                pNOM = new NoOptic(this->sky_.stars_,para);
//                ModelEvaluation eval1=this->ComprehensiveEvaluation(model,round,miss_num,add_num,off_rate);
//                output += "CP=" + to_string(cp)+" and "+"CP="+to_string(rp) +":"+ to_string(eval1.accuracy)+"\n";
//                cout<<output<<endl;
//                cout.flush();
//            }
//        }
        delete pNOM;
        pNOM = nullptr;
        round = sround;
    }


    ModelEvaluation eval1=this->ComprehensiveEvaluation(model,round,miss_num,add_num,off_rate);
    this->SIMULATE = false;
    cout<<output<<endl;
    qDebug()<<QString::fromStdString(output)<<endl;
    qDebug("End.");
    return eval1;
}

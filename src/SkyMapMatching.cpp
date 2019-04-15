//
// Created by 70700 on 2019/3/3.
//

#include "SkyMapMatching.h"
#include <fstream>
void SkyMapMatching::LoadSky(QString &f_name) {
    QCSVAdapter csv_sky(f_name);
    //int count = 0;
    if(!this->sky_.stars_.empty()) this->sky_.stars_.clear();
    this->sky_.stars_=csv_sky.getRecords();
    this->sky_.count_=this->sky_.stars_.size();
    this->sky_.range_ = {360,180};
    this->sky_.centre_ = StarPoint(-1,180.0,0,0);
}

void Observation::setProperties(image_properties prop){
    this->imageWidth = prop.imageWidth;
    this->imageHeight = prop.imageHeight;
    this->imageWidthL = prop.imageWidthL;
    this->imageHeightL = prop.imageHeightL;
    this->focal_length = prop.focal_length;
    this->range_ = {prop.imageWidthL,prop.imageHeightL};
}

void SkyMapMatching::LoadImage(QString &f_name,image_properties property) {

    QCSVAdapter csv_sky(f_name);
    vector<StarPoint> stars = csv_sky.getRecords();
    //TO DO:change the pixel to angular_distance...
    //need the size of the picture...
    this->image_.setProperties(property);
    double max_x=0,max_y=0;
    for(StarPoint sp:stars){
        max_x = max(max_x,sp.x);
        max_y = max(max_y,sp.y);
    }
    cout<<"Max_x:"<<max_x<<endl;
    cout<<"Max_y:"<<max_y<<endl;
    cout<<" ImageWidthL:"<<property.imageWidthL<<endl;
    cout<<"ImageHeightL:"<<property.imageHeightL<<endl;
    for(size_t i=0;i<stars.size();i++){
        stars[i].x *= property.imageWidthL/property.imageWidth;
        stars[i].y *= property.imageHeightL/property.imageHeight;
        cout<<i<<"th:("<<stars[i].x<<","<<stars[i].y<<")"<<endl;
    }
    if(!this->image_.stars_.empty()) this->image_.stars_.clear();
    this->image_.stars_ = stars;
    this->image_.count_=this->image_.stars_.size();
    this->image_.RangeStandardization();
    cout<<"image true range: ("<<this->image_.range_.first<<","<<this->image_.range_.second<<")"<<endl;

    StarPoint center(0,this->image_.range_.first/2,this->image_.range_.second/2,0);
    for(size_t i=0;i<this->image_.count_;i++){
        this->image_.stars_[i].change_coordinate(center);
    }

}

void SkyMapMatching::SelectTargetStar() {
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
}

void SkyMapMatching::SelectTargetStar(int target) {
    this->__target_star = this->image_.stars_[size_t(target)];
}

int SkyMapMatching::TriangleModel() {
    assert(this->image_.count_>=3);
    cout<<"Belowing are all-possible angle_distance in this image."<<endl;
    cout<<"focal_length:"<<this->image_.focal_length<<endl;
    if(this->image_.focal_length>1e-9){
        for(size_t i=0;i<this->image_.count_;i++){
            for(size_t j=i+1;j<this->image_.count_;j++){
                double angle_distance = getSpotAD(this->image_.stars_[i].x,this->image_.stars_[i].y,
                                                  this->image_.stars_[j].x,this->image_.stars_[j].y,
                                                  this->image_.focal_length);
                string buf = to_string(i)+"--"+to_string(j)+" : "+to_string(angle_distance);
                cout<<buf<<endl;
            }
        }
    }else{
        cout<<"There is no focal_length info from this image!"<<endl;
    }

    TriangleMatching TM(sky_.stars_.size(), 12.0, 0.02);
    TM.LoadData(sky_.stars_);

    vector<StarPoint> triangle;
    triangle.push_back(this->__target_star);

    double dis12=0.0,dis13=0.0,dis23=0.0;
    int round=0;
    while(round++<100){
        TM.ChooseAdjacentStars(image_.stars_,triangle);
        if(this->image_.focal_length > 1e-9){
            dis12 = getSpotAD(triangle[0].x,triangle[0].y,triangle[1].x,triangle[1].y,this->image_.focal_length);
            dis13 = getSpotAD(triangle[0].x,triangle[0].y,triangle[2].x,triangle[2].y,this->image_.focal_length);
            dis23 = getSpotAD(triangle[1].x,triangle[1].y,triangle[2].x,triangle[2].y,this->image_.focal_length);
        }else {
            double diagonal = sqrt(pow(this->image_.imageWidthL,2)+pow(this->image_.imageHeightL,2));
            dis12 = 20*cal_dis(triangle[0].x,triangle[0].y,triangle[1].x,triangle[1].y)/diagonal;
            dis13 = 20*cal_dis(triangle[0].x,triangle[0].y,triangle[2].x,triangle[2].y)/diagonal;
            dis23 = 20*cal_dis(triangle[1].x,triangle[1].y,triangle[2].x,triangle[2].y)/diagonal;
        }

        cout<<"dis12    "<<"dis13   "<<"dis23"<<endl;
        cout<<dis12<<"  "<<dis13<<" "<<dis23<<endl;
        if(dis12 < TM.GetThreshold() && dis13 < TM.GetThreshold() && dis23 < TM.GetThreshold()) break;
        triangle.pop_back();
        triangle.pop_back();
    }
    if(round>=100){
        cout<<"Trangle Model cannot find proper adjacent stars."<<endl;
        return -1;
    }
    int result_number = TM.MatchAlgorithm(dis12,dis13,dis23,triangle[0].magnitude,triangle[1].magnitude,triangle[2].magnitude);
    cout<<"Match Ended!"<<endl;
    return (result_number-1); //编号和index差1
}

int SkyMapMatching::NoOpticModel(){
    NoOptic NOM(this->sky_.stars_,this->image_.stars_);
    int result = NOM.ExeNoOptic(this->__target_star.index);
    cout<<"NoOptic Model end:"<<result<<endl;
    return (result);
}

void SkyMapMatching::Match(size_t model) {
    if(!this->candidates_.empty()) this->candidates_.clear();
    switch (model) {
    case 1:{
        int skymap_index2 = TriangleModel();
        if(skymap_index2 >= 0) {
            printf("Triangle Model Result: %d \n",skymap_index2);
            this->__matching_star = this->sky_.stars_[size_t(skymap_index2)];
            Candidate one("Triangle Model",this->__matching_star);
            this->candidates_.push_back(one);
        }
        else printf("Triangle Model cannot get answer.\n");
        fflush(stdin);
        break;
    }
    case 2:{
        int skymap_index1 = NoOpticModel();
        if(skymap_index1 >= 0) {
            printf("NoOptic Model Result: %d\n",skymap_index1);
            this->__matching_star = this->sky_.stars_[size_t(skymap_index1)];
            Candidate one("NoOptic Model",this->__matching_star);
            this->candidates_.push_back(one);
        }
        else printf("NoOptic Model cannot get answer.\n");
        fflush(stdin);

        int skymap_index2 = TriangleModel();
        if(skymap_index2 >= 0) {
            printf("Triangle Model Result: %d \n",skymap_index2);
            this->__matching_star = this->sky_.stars_[size_t(skymap_index2)];
            Candidate one("Triangle Model",this->__matching_star);
            this->candidates_.push_back(one);
        }
        else printf("Triangle Model cannot get answer.\n");
        fflush(stdin);
        break;
    }
    default:{
        int skymap_index1 = NoOpticModel();
        if(skymap_index1 >= 0) {
            printf("NoOptic Model Result: %d\n",skymap_index1);
            this->__matching_star = this->sky_.stars_[size_t(skymap_index1)];
            Candidate one("NoOptic Model",this->__matching_star);
            this->candidates_.push_back(one);
        }
        else printf("NoOptic Model cannot get answer.\n");
        fflush(stdin);

        int skymap_index2 = TriangleModel();
        if(skymap_index2 >= 0) {
            printf("Triangle Model Result: %d \n",skymap_index2);
            this->__matching_star = this->sky_.stars_[size_t(skymap_index2)];
            Candidate one("Triangle Model",this->__matching_star);
            this->candidates_.push_back(one);
        }
        else printf("Triangle Model cannot get answer.\n");
        fflush(stdin);

        if(skymap_index1<=0 && skymap_index2<=0) {
            printf("No Model get answer.\n");
            fflush(stdin);
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
    if(abs(s1.Distance(s2)) <0.001) return true;
    else return false;
}

bool similar_vector(vector<StarPoint> &vec1, vector<StarPoint> &vec2){
    /*TODO:
     * compare two vector,
     * without consideration of the absolute position, but the relative position.*/
    return true;
}

int SkyMapMatching::Check() {
    //get the __matching_star's offset in check_set, which should be equal to offset of __target_star in sky_image.
    StarPoint check_center = this->__matching_star;
    check_center.x -= this->__target_star.x;
    check_center.y -= this->__target_star.y;
    vector<StarPoint> check_set = this->sky_.Subset(check_center,this->image_.range_.first,this->image_.range_.second);
    for(size_t i=0;i<check_set.size();i++){
        check_set[i].change_coordinate(this->__matching_star);
    }

    sort(check_set.begin(),check_set.end(),star_point_cmp);

    vector<StarPoint> candidate_set(this->image_.stars_);
    StarPoint new_centre = this->__target_star;
    for(size_t i=0;i<candidate_set.size();i++){
        candidate_set[i].change_coordinate(new_centre);
//        s.x -= new_centre.x;
//        s.y -= new_centre.y;
    }
    sort(candidate_set.begin(),candidate_set.end(),star_point_cmp);

    size_t len = min(check_set.size(),candidate_set.size());
    double loss = 0;
    size_t i=0,j=0;
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
        cout<<loss<<" ";
    }
    cout<<endl;
    cout<<"Finale Loss:"<<loss<<endl;
    if(loss/len > 0.5) return -1;
    return this->__matching_star.index;
}

int SkyMapMatching::CheckAllCandidates(){
    assert(this->candidates_.size()>0);
    int passnum = 0;
    int re_index = -1;
    for(Candidate one:this->candidates_){
        StarPoint sp = one.star;
        this->__matching_star = sp;
        cout<<"Checking answer from"<<one.model_name<<"::"<<sp.index<<" ("<<sp.x<<","<<sp.y<<") "<<sp.magnitude<<endl;
        int pass = Check();
        if(pass == -1){
            cout<<"This candidate didn't pass checking"<<endl;
        }else{
            cout<<"Passed!"<<endl;
            re_index = sp.index;
            passnum++;
        }
    }
    cout<<"total passed:"<<passnum<<endl;
    return re_index;
    //return passnum;
}

vector<StarPoint> SkyMap::Subset(StarPoint centre, double length, double width) {
    //Some Check here.
    vector<StarPoint> stars;
    //cout<<"subset of..."<<stars_.size()<<endl;
    for(StarPoint point : stars_){
        if(point.InRange(centre,length,width)){
            stars.push_back(point);
        }
    }
    //cout<<"Subset(length,width)=("<<length<<","<<width<<"):"<<endl;
    //cout<<"Number of stars:"<<stars.size()<<endl;
    return stars;

}

void SkyMapMatching::GenerateSimImage(StarPoint centre, double length, double width) {
    //Some Check here.
    cout<<"Generating...::"<<length<<","<<width<<endl;
    if(length<=0 || width<=0 ){
        if(!this->image_.stars_.empty()) this->image_.stars_.clear();
        this->image_.count_ = 0;
        this->image_.range_ = {length<0?0:length , width<0?0:width};
        return;
    }
    vector<StarPoint> stars = this->sky_.Subset(centre,length,width);
    for(size_t i=0;i<stars.size();i++){
        stars[i].change_coordinate(centre);
    }
    if(!this->image_.stars_.empty()) this->image_.stars_.clear();

    //this->image_.range_ = {length , width};
    this->image_.stars_.insert(this->image_.stars_.end(),stars.begin(),stars.end());
    this->image_.count_ = this->image_.stars_.size();
    this->image_.centre_ = centre;
    image_properties prop(0,0,length,width,0.0);
    this->image_.setProperties(prop);

    cout<<"The Generated image is:"<<endl;
    cout<<"*center: "<<centre.index<<"-> ("<<centre.x<<" , "<<centre.y<<")"<<endl;
    cout<<"*range : "<<length<<" * "<<width<<endl;
    cout<<"*The number of stars is:"<<stars.size()<<endl;
    for(size_t i=0;i<stars.size();i++){
        cout<<i<<"th--> "<<stars[i].index<<": ("<<stars[i].x<<" , "<<stars[i].y<<")"<<endl;
        //the index will change into image_id.
        this->sky_.stars_[i].index = static_cast<int>(i);
    }
    //this->image_.RangeStandardization();
    cout<<endl;
}

vector<StarPoint> SkyMap::Subset(StarPoint centre, double image_ratio, int numi=0) {
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
        cout<<"The number of star of this Subset:"<<stars.size()<<endl;
    } else{
        cout<<"Invalid subset! You can try another ratio or center."<<endl;
        stars.clear();
    }
    return stars;
}

void SkyMapMatching::GenerateSimImage(StarPoint centre, double image_ratio, int num) {
    vector<StarPoint> stars = sky_.Subset(centre,image_ratio,num);
    if(!this->image_.stars_.empty()) this->image_.stars_.clear();
    if(stars.size() == size_t(num)) {
        for(size_t i=0;i<stars.size();i++){
            stars[i].change_coordinate(centre);
//            stars[i].x -= centre.x;
//            stars[i].y -= centre.y;
        }
        this->image_.stars_.insert(this->image_.stars_.end(),stars.begin(),stars.end());
        this->image_.count_ = this->image_.stars_.size();
        this->image_.centre_ = centre;

        cout<<"The Generated image is:"<<endl;
        for(size_t i=0;i<stars.size();i++){
            cout<<stars[i].index<<": ("<<stars[i].x<<" , "<<stars[i].y<<")"<<endl;
        }
        cout<<"The number of stars is:"<<stars.size()<<endl;

    } else{
        this->image_.count_ = 0;
        this->image_.centre_ = centre;
        cout<<"Invalid Subset! You can try another ratio or center."<<endl;
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

ModelEvaluation SkyMapMatching::TriangleModelSimulation(int round){
    //no different to ExeSimulation..
    cout<<round<<endl;
    assert(round>0);
    int succeed_num=0;
    int failed_num = 0;
    while(round-->0){
        StarPoint center = random_point(0.0,this->LongitudeRange,- this->LatitudeRange/2,this->LatitudeRange/2);
        this->GenerateSimImage(center,15.0,15.0);
        size_t target = central_star(this->image_.stars_);
        this->__target_star = this->image_.stars_[target];
        this->Match(1);
        if(this->CheckAllCandidates() == -1){
            failed_num ++;
        }
        else {
            succeed_num ++;
        }
    }
    double ans = (succeed_num+1.0)/(succeed_num+failed_num);
    ModelEvaluation eval(succeed_num+failed_num,ans,"Triangle Model");
    return eval;

}

ModelEvaluation SkyMapMatching::NoOpticModelSimulation(int round){
    cout<<round<<endl;
    assert(round>0);
    ModelEvaluation eval;
    return eval;
}

ModelEvaluation SkyMapMatching::SimpleEvaluation(size_t model, int round){
    //simple test.
    cout<<round<<endl;
    assert(round>0);
    int succeed_num=0;
    int failed_num = 0;
    while(round-->0){
        StarPoint center = random_point(0.0,this->LongitudeRange,- this->LatitudeRange/2,this->LatitudeRange/2);
        this->GenerateSimImage(center,15.0,15.0);
        size_t target = central_star(this->image_.stars_);
        this->__target_star = this->image_.stars_[target];
        this->Match(model);
        if(this->CheckAllCandidates() == -1){
            failed_num ++;
        }
        else {
            succeed_num ++;
        }
    }
    double ans = (succeed_num+1.0)/(succeed_num+failed_num);
    ModelEvaluation eval(succeed_num+failed_num,ans,"Triangle Model");
    return eval;
}

void RandomMissing(vector<StarPoint> &stars, size_t miss_num){
    assert(miss_num>=0 && miss_num<stars.size());
    if(miss_num==0) return;
    vector<size_t> missed(miss_num);
    //for(size_t i=0;i<miss_num;i++) missed[i]=i;
    random_shuffle(stars.begin(),stars.end());
    while(miss_num-->0) stars.pop_back();
}

void Observation::ContentSync(){
    this->count_ = this->stars_.size();
}

ModelEvaluation SkyMapMatching::MissingEvaluation(size_t model, int round, size_t miss_num){
    //missing test.
    cout<<round<<endl;
    assert(round>0);
    int succeed_num=0;
    int failed_num = 0;
    while(round-->0){
        StarPoint center = random_point(0.0,this->LongitudeRange,- this->LatitudeRange/2,this->LatitudeRange/2);
        this->GenerateSimImage(center,15.0,15.0);
        //missing.....
        RandomMissing(this->image_.stars_,miss_num);
        this->image_.count_ -= miss_num;

        size_t target = central_star(this->image_.stars_);
        this->__target_star = this->image_.stars_[target];
        this->Match(model);
        if(this->CheckAllCandidates() == -1){
            failed_num ++;
        }
        else {
            succeed_num ++;
        }
    }
    double ans = (succeed_num+1.0)/(succeed_num+failed_num);
    ModelEvaluation eval(succeed_num+failed_num,ans,"Triangle Model");
    return eval;
}

void RandomAddPoints(vector<StarPoint> &stars,double length, double width, size_t redundence_num){
    assert(redundence_num>=0);
    if(redundence_num==0) return;
    while(redundence_num-->0){
        StarPoint sp = random_point(-length/2,length/2,-width/2,width/2);
        stars.push_back(sp);
    }
}

ModelEvaluation SkyMapMatching::RedundanceEvaluation(size_t model, int round, size_t add_num){
    //redundence test.
    cout<<round<<endl;
    assert(round>0);
    int succeed_num=0;
    int failed_num = 0;
    while(round-->0){
        StarPoint center = random_point(0.0,this->LongitudeRange,- this->LatitudeRange/2,this->LatitudeRange/2);
        this->GenerateSimImage(center,15.0,15.0);

        //adding new points.....
        RandomAddPoints(this->image_.stars_,this->image_.imageWidthL,this->image_.imageHeightL,add_num);
        this->image_.count_ += add_num;

        size_t target = central_star(this->image_.stars_);
        this->__target_star = this->image_.stars_[target];
        this->Match(model);
        if(this->CheckAllCandidates() == -1){
            failed_num ++;
        }
        else {
            succeed_num ++;
        }
    }
    double ans = (succeed_num+1.0)/(succeed_num+failed_num);
    ModelEvaluation eval(succeed_num+failed_num,ans,"Triangle Model");
    return eval;
}

void RandomDiviation(vector<StarPoint> &stars, double off_rate, size_t type=0){
    assert(off_rate>=0);
    if(abs(off_rate)<1e-9) return;
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

ModelEvaluation SkyMapMatching::DeviationEvaluation(size_t model, int round, double offset_rate){
    //deviation test.
    cout<<round<<endl;
    assert(round>0);
    int succeed_num=0;
    int failed_num = 0;
    while(round-->0){
        StarPoint center = random_point(0.0,this->LongitudeRange,- this->LatitudeRange/2,this->LatitudeRange/2);
        this->GenerateSimImage(center,15.0,15.0);

        //give deviation.....
        RandomDiviation(this->image_.stars_,offset_rate);

        size_t target = central_star(this->image_.stars_);
        this->__target_star = this->image_.stars_[target];
        this->Match(model);
        if(this->CheckAllCandidates() == -1){
            failed_num ++;
        }
        else {
            succeed_num ++;
        }
    }
    double ans = (succeed_num+1.0)/(succeed_num+failed_num);
    ModelEvaluation eval(succeed_num+failed_num,ans,"Triangle Model");
    return eval;
}

ModelEvaluation SkyMapMatching::ComprehensiveEvaluation(size_t model, int round, size_t miss_num,
                                                        size_t add_num, double offset_rate){
    //comprehensive test.
    cout<<endl<<endl<<endl<<endl;
    cout<<"Evaluation start...."<<endl;
    cout<<"Total round number:"<<round<<endl;
    assert(round>0);
    int succeed_num=0;
    int failed_num = 0;
    int r=0;
    round = 5;
    while(r<round){
        cout<<"--------------------------------------------------------------"<<endl;
        cout<<"-----------------------start:"<<r+1<<"th---------------------------"<<endl;
        StarPoint center = random_point(0.0,this->LongitudeRange,- this->LatitudeRange/2,this->LatitudeRange/2);
        this->GenerateSimImage(center,15.0,15.0);
        if(this->image_.count_>=3){
            //add noise....
            cout<<"@Adding noise..."<<endl;
            RandomMissing(this->image_.stars_,miss_num);
            this->image_.count_ -= miss_num;
            RandomAddPoints(this->image_.stars_,this->image_.imageWidthL,this->image_.imageHeightL,add_num);
            this->image_.count_ += add_num;
            RandomDiviation(this->image_.stars_,offset_rate);

            size_t target = central_star(this->image_.stars_);
            this->__target_star = this->image_.stars_[target];
            cout<<"@Matching..."<<endl;
            this->Match(model);
            cout<<endl<<"@Checking..."<<endl;
            if(this->CheckAllCandidates() == -1){
                failed_num ++;
            }
            else {
                succeed_num ++;
            }
            cout.flush();
            r++;
            cout<<"-----------------------end:"<<r<<"th---------------------------"<<endl;
        }
        else cout<<"-----------------------end:"<<r+1<<"th---------------------------"<<endl;
        cout<<"--------------------------------------------------------------"<<endl;
    }
    double ans = (succeed_num+1.0)/(succeed_num+failed_num);
    ModelEvaluation eval(succeed_num+failed_num,ans,"Triangle Model");
    return eval;
}

ModelEvaluation SkyMapMatching::ExeSimulation(size_t model,int round,size_t miss_num,
                                              size_t add_num,double off_rate){
    //simple test.
    ModelEvaluation eval=this->ComprehensiveEvaluation(model,round,miss_num,add_num,off_rate);
    return eval;
    /*
    switch (model) {
    case 1:{
        return TriangleModelSimulation(round);
        //break;
    }
    case 2:{
        return NoOpticModelSimulation(round);
        //break;
    }
    default:{
        cout<<"Default evalution model: Triangle Model."<<endl;
        return TriangleModelSimulation(round);
    }
    }*/

    //test with missing noise

}

//void SkyMapMatching::initPara(int w,int h,double wl,double hl,double f)
//{
//    this->image_.imageWidth=w;
//    this->image_.imageHeight=h;
//    this->image_.imageWidthL=wl;
//    this->image_.imageHeightL=hl;
//    if(fabs(f)<1e-9)
//        this->image_.focal_length=f;
//    else
//        this->image_.focal_length=12;
//}

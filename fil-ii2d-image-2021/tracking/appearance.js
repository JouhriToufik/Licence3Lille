var appearance_tracking={}

/*
appearance_tracking.MeanShift - définit un prototype pour suivre le déplacement de l'objet contenu dans la boîte englobante ("bbox"). à chaque invocation de process, des regions rectangulaires sont générées autour de la bbox pour identifier un éventuel déplacement de l'objet contenu initialement dans la "bbox". afin d'assurer le suivis, des descripteurs pour décrire le contenu de la boîte englobante et des regions candidates pour la suite de suivi sont extraits avec la fonction "per_region_feature_func". la similarité entre descripteurs est mesurée par  "metric_func"
*/
appearance_tracking.MeanShift=function(bbox,per_region_feature_func,metric_func,opt_options){

  this.bbox=bbox;

  this.window_width=opt_options&&opt_options.window_width?opt_options.window_width:3*bbox.dx;
  this.window_height=opt_options&&opt_options.window_height?opt_options.window_height:3*bbox.dy;

  this.step_x=opt_options&&opt_options.step_x?opt_options.step_x:bbox.dx/3;
  this.step_y=opt_options&&opt_options.step_y?opt_options.step_y:bbox.dy/3;

  this.per_region_feature_func=per_region_feature_func;
  this.metric_func=metric_func;

  this.threshold=opt_options&&opt_options.threshold?opt_options.threshold:Number.MAX_SAFE_INTEGER;

  this.count=0;

  this.update_model=opt_options&&opt_options.update_model?opt_options.update_model:false;
}

appearance_tracking.MeanShift.prototype.process=function(in_imgData,out_imgData) {

  var in_width=in_imgData.width, in_height=in_imgData.height;

  //BLOC 1
  // on choisit un feature s'il y'a pas
  if (!this.bbox_feature) {
    var _opt_options=this.bbox;
    this.bbox_feature=this.per_region_feature_func(in_imgData,_opt_options);
  }


  //BLOC 2
  //la taille du carré de recherche
  this.pan_x=Math.round(this.window_width/2-this.bbox.dx/2);
  this.pan_y=Math.round(this.window_height/2-this.bbox.dy/2);

  //BLOC 3
  // position de depart du carré de recherche
  var x_start=(this.bbox.x0-this.pan_x)>0?(this.bbox.x0-this.pan_x):0;
  var y_start=(this.bbox.y0-this.pan_y)>0?(this.bbox.y0-this.pan_y):0;
  var x_end=(this.bbox.x0+this.bbox.dx+this.pan_x) < in_width?
            (this.bbox.x0+this.pan_x)
            :(in_width-this.bbox.dx);
  var y_end=(this.bbox.y0+this.bbox.dy+this.pan_y)<in_height?
            this.bbox.y0+this.pan_y
            :(in_height-this.bbox.dy);

  var min=Number.MAX_SAFE_INTEGER;
  var min_bbox,min_bbox_feature;
  var count_y=in_imgData.height;

  //BLOC 4
  //déssine la partie data image
  if (out_imgData) {
    Tools.copy_partial_imageData_into_imageData(in_imgData,
                0,0,in_imgData.width,in_imgData.height,
                out_imgData,0,0);
    Tools.strokeBBox_on_imageData(out_imgData,{x0:x_start,y0:y_start,dx:x_end-x_start,dy:y_end-y_start},[64,64,64,255]);
  }

  //BLOC 5
  //desinne ce que le carré de recherche a trouvé
  for (var y=y_start;y<y_end;y+=this.step_y) {
    var count_x=0;
    for (var x=x_start;x<x_end;x+=this.step_x) {
      var local_bbox={
                    x0:(x+this.bbox.dx<in_width)?x:in_width-this.bbox.dx,
                    y0:(y+this.bbox.dy<in_height)?y:in_height-this.bbox.dy,
                    dx:this.bbox.dx,
                    dy:this.bbox.dy
                  }
      if (out_imgData) {
        Tools.copy_partial_imageData_into_imageData(in_imgData,
                    local_bbox.x0,local_bbox.y0,local_bbox.dx,local_bbox.dy,
                    out_imgData,count_x,count_y);
        Tools.strokeBBox_on_imageData(out_imgData,local_bbox,[0,64,64,255]);
     }

      //BLOC 6
      //la difference entre l'image et ce qu'il trouve
      var local_bbox_feature=this.per_region_feature_func(in_imgData,local_bbox);
      if (!local_bbox_feature) continue;
      var diff=this.metric_func(this.bbox_feature,local_bbox_feature);
      if (diff < min) {
        min=diff;
        min_bbox=local_bbox;
        min_bbox_feature=local_bbox_feature;
      }

      count_x+=this.bbox.dx;
    }
    count_y+=this.bbox.dy;
  }

  //BLOC 7
  //mise a jours du modele par rapport a ce qu'il a detecté
  if (min < this.threshold && this.bbox!=min_bbox) {
      this.count++;
      this.bbox=min_bbox;
      if (this.update_model)
        this.bbox_feature=min_bbox_feature;
  }

  if (out_imgData) {
    Tools.strokeBBox_on_imageData(out_imgData,this.bbox,[255,0,255,255]);
  }
  return this.bbox;
}

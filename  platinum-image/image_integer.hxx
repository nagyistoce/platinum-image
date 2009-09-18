// This file is part of the Platinum library.
// Copyright (c) 2007 Uppsala University.
//
//    The Platinum library is free software; you can redistribute it and/or modify
//    it under the terms of the GNU Lesser General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    The Platinum library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Lesser General Public License for more details.
//
//    You should have received a copy of the GNU Lesser General Public License
//    along with the Platinum library; if not, write to the Free Software
//    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#ifndef __image_integer_hxx__
#define __image_integer_hxx__

#define WSHED 0
#define MASK -2
#define INIT -1

#include "image_integer.h"
#include "image_scalar.hxx"

// *** Processing ***

template <class ELEMTYPE, int IMAGEDIM>
image_integer<short, 3>* image_integer<ELEMTYPE, IMAGEDIM>::watershed()
{
	image_integer<short, 3>* res = new image_integer<short, 3>(this);
	res->fill(INIT);
	int current_label=0;
	int current_dist;
	image_integer<unsigned short, 3>* dist_image = new image_integer<unsigned short, 3>(this);
	dist_image->fill(0);

	// Sort voxels by grayscale level
	int xsize = this->get_size_by_dim(0);
	int ysize = this->get_size_by_dim(1);
	int zsize = this->get_size_by_dim(2);
	map<ELEMTYPE, vector<Vector3Dint> > histogram;
	for (int z=0; z<zsize; z++)
		for (int y=0; y<ysize; y++)
			for (int x=0; x<xsize; x++)
				histogram[this->get_voxel(x,y,z)].push_back(create_Vector3Dint(x,y,z));
			
	Vector3Dint p, p1, p2;
	queue<Vector3Dint> fifo;
	Vector3Dint fictious_voxel = create_Vector3Dint(-1, -1, -1);
	short res_val, res_val1;
	bool break_flag;
	typename map<ELEMTYPE, vector<Vector3Dint> >::iterator hist_iter;
	vector<Vector3Dint>::iterator level_iter;
	for (hist_iter = histogram.begin(); hist_iter!=histogram.end(); hist_iter++) {
		// Geodesic SKIZ of level h-1 inside level h
		for (level_iter = hist_iter->second.begin(); level_iter!=hist_iter->second.end(); level_iter++) {
			p = *level_iter;
			res->set_voxel(p,MASK);
			// search neighbourhood in res
			break_flag=false;
			for (int a=max(p[0]-1, 0); a<=min(p[0]+1, xsize-1); a++) {
				for (int b=max(p[1]-1, 0); b<=min(p[1]+1, ysize-1); b++) {
					for (int c=max(p[2]-1, 0); c<=min(p[2]+1, zsize-1); c++) {
						res_val = res->get_voxel(a,b,c);
						if (res_val>0 || res_val==WSHED) {
							dist_image->set_voxel(p,1);
							fifo.push(p);
							break_flag=true;
							break;
						}
					}
					if (break_flag)
						break;
				}
				if (break_flag)
					break;
			}
		}
		
		current_dist=1;
		fifo.push(fictious_voxel);
		while (true) {
			p = fifo.front();
			fifo.pop();
			if (p==fictious_voxel) {
				if (fifo.empty())
					break;
				else {
					fifo.push(fictious_voxel);
					current_dist++;
					p=fifo.front();
					fifo.pop();
				}
			}
			res_val = res->get_voxel(p);
			for (int a=max(p[0]-1, 0); a<=min(p[0]+1, xsize-1); a++) {
				for (int b=max(p[1]-1, 0); b<=min(p[1]+1, ysize-1); b++) {
					for (int c=max(p[2]-1, 0); c<=min(p[2]+1, zsize-1); c++) {
						p1 = create_Vector3Dint(a,b,c);
						res_val1 = res->get_voxel(p1);
						if (dist_image->get_voxel(p1) < current_dist && (res_val1>0 || res_val1==WSHED) ) {
							// i.e., p1 belongs to an already labeled basin or to the watersheds
							if (res_val1>0) {
								if (res_val==MASK || res_val==WSHED)
									res->set_voxel(p, res_val1);
								else if (res_val!=res_val1)
									res->set_voxel(p,WSHED);
							}
							else if (res_val==MASK)
								res->set_voxel(p,WSHED);
						}
						else if (res_val1==MASK && dist_image->get_voxel(p1)==0) {
							dist_image->set_voxel(p1, current_dist+1);
							fifo.push(p1);
						}
					}
				}
			}
		}
	
		// Checks if new minima have been discovered
		for (level_iter = hist_iter->second.begin(); level_iter!=hist_iter->second.end(); level_iter++) {
			p = *level_iter;
			dist_image->set_voxel(p, 0); // the distance associated with p is reset to 0
			if (res->get_voxel(p)==MASK) {
				current_label++;
				fifo.push(p);
				res->set_voxel(p, current_label);
				while (!fifo.empty()) {
					p1=fifo.front();
					fifo.pop();
					for (int a=max(p1[0]-1, 0); a<=min(p1[0]+1, xsize-1); a++) {
						for (int b=max(p1[1]-1, 0); b<=min(p1[1]+1, ysize-1); b++) {
							for (int c=max(p1[2]-1, 0); c<=min(p1[2]+1, zsize-1); c++) {
								p2 = create_Vector3Dint(a,b,c);
								if (res->get_voxel(p2)==MASK) {
									fifo.push(p2);
									res->set_voxel(p2,current_label);
								}
							}
						}
					}
				}
			}
		}
	}

	delete dist_image;
	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
vector<ELEMTYPE> image_integer<ELEMTYPE, IMAGEDIM>::get_distinct_values_in_slice_2D(int slice, int direction, bool ignore_zeroes)
{
	ELEMTYPE p;
	vector<ELEMTYPE> values;
	typename vector<ELEMTYPE>::iterator iterator;
	for (int u=0; u<this->get_size_by_dim_and_dir(0,direction); u++) {
		for (int v=0; v<this->get_size_by_dim_and_dir(1,direction); v++) {
			p=this->get_voxel_by_dir(u,v,slice,direction);
			if (!ignore_zeroes || p!=0) {
				iterator = values.begin();
				while( iterator != values.end() && *iterator != p)
					iterator++;
				if (iterator==values.end())
					values.push_back(p);
			}
		}
	}
	return values;
}

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_integer<ELEMTYPE, IMAGEDIM>::gauss_fit2()
    {
	this->min_max_refresh();
	ELEMTYPE min_val=this->get_min();
	ELEMTYPE max_val=this->get_max();
	ELEMTYPE err_min_ind=max_val;
	double* hist=new double[1+max_val-min_val];
	memset(hist, 0, sizeof(double)*(1+max_val-min_val));

    typename image_storage<ELEMTYPE >::iterator i = this->begin();
    
    while (i != this->end()) //images are same size and should necessarily end at the same time
        {
        hist[*i-min_val]++;
        ++i;
        }

	//Test all threshold values
	double sum2_error_min=-10000001;
	ELEMTYPE sum2_error_min_ind=min_val;
	double sum1=0;
	double sum2=0;
	double sum1_2=0;
	double sum2_2=0;
	double n1=0;
	double n2=0;
	double cumSum=0;
	double cumSum_2=0;
	double cumN=0;
	double sum1opt=0;
	double sum2opt=0;
	double sum1_2opt=0;
	double sum2_2opt=0;
	double n1opt=0;
	double n2opt=0;
	double std1opt=0;
	double std2opt=0;
	double mean1opt=0;
	double mean2opt=0;

	unsigned long j;
	for(j=min_val; j<=max_val; j++)
		{
		cumSum+=j*hist[j-min_val];
		cumSum_2+=j*j*hist[j-min_val];
		cumN+=hist[j-min_val];
		}

	for(j=min_val; j<=max_val; j++)
		{
		//Compute mean and std
		sum1+=j*hist[j-min_val];
		sum1_2+=j*j*hist[j-min_val];
		n1+=hist[j-min_val];
		sum2=cumSum-sum1;
		sum2_2=cumSum_2-sum1_2;
		n2=cumN-n1;

		if(n1>1 && n2>1)
			{
			double mean1=sum1/n1;
			double mean2=sum2/n2;
			double std1=sqrt((sum1_2-n1*mean1*mean1)/(n1-1));
			double std2=sqrt((sum2_2-n2*mean2*mean2)/(n2-1));
			double p1=n1/(n1+n2);
			double p2=n2/(n1+n2);
			if(std1>0 && std2>0)
				{
				double sum2error=p1*log(std1)+p2*log(std2)-p1*log(p1)-p2*log(p2);
				if(sum2error<sum2_error_min || sum2_error_min<-10000000)
					{
					sum2_error_min=sum2error;
					sum2_error_min_ind=j+1;
					sum1opt=sum1;
					sum2opt=sum2;
					sum1_2opt=sum1_2;
					sum2_2opt=sum2_2;
					n1opt=n1;
					n2opt=n2;
					std1opt=std1;
					std2opt=std2;
					mean1opt=mean1;
					mean2opt=mean2;
					}    
				}
			}
		}

	double* gauss1hist=new double[1+max_val-min_val];
	double* gauss2hist=new double[1+max_val-min_val];
	double* gausstothist=new double[1+max_val-min_val];
	memset(gauss1hist, 0, sizeof(double)*(1+max_val-min_val));
	memset(gauss2hist, 0, sizeof(double)*(1+max_val-min_val));
	memset(gausstothist, 0, sizeof(double)*(1+max_val-min_val));

	//Compute mean and std
	sum1=sum1opt;
	sum2=sum2opt;
	sum1_2=sum1_2opt;
	sum2_2=sum2_2opt;
	n1=n1opt;
	n2=n2opt;

	if(n1>1 && n2>1)
		{
		double sum2diffMin=0;
		double mean1=sum1/n1;
		double mean2=sum2/n2;
		double std1=sqrt((sum1_2-n1*mean1*mean1)/(n1-1));
		double std2=sqrt((sum2_2-n2*mean2*mean2)/(n2-1));
		double sqrt2pi=sqrt(2*pt_PI);
		double sumGauss1=0;
		double sumGauss2=0;
		for(j=min_val; j<=max_val; j++)
			{
			gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
			gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
			sumGauss1+=gauss1hist[j-min_val];
			sumGauss2+=gauss2hist[j-min_val];
			}
		for(j=min_val; j<=max_val; j++)
			{
			gauss1hist[j-min_val]*=n1/sumGauss1;
			gauss2hist[j-min_val]*=n2/sumGauss2;
			gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
			sum2diffMin+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
			}
		double dN1=n1opt*0.5;
		double dN2=n2opt*0.5;
		double dM1=std1opt;
		double dM2=std2opt;
		double dS1=std1opt/2;
		double dS2=std2opt/2;
		double sum2diff1=0;
		double sum2diff2=0;
		n1=n1opt;
		n2=n2opt;
		mean1=mean1opt;
		mean2=mean2opt;
		std1=std1opt;
		std2=std2opt;
		double minDN1=n1opt*0.001;
		double minDN2=n2opt*0.001;
		double minDS1=0.5;
		double minDS2=0.5;
		double minDM1=0.5;
		double minDM2=0.5;
		double decrease=0.5;
	
		while(dN1>=minDN1 || dN2>=minDN2 || dM1>=minDM1 || dM2>=minDM2 || dS1>=minDS1 || dS2>=minDS2)
			{
			if(dN1>=minDN1)
				{
				//Test n1
				n1=n1opt-dN1;
				n2=n2opt;
				mean1=mean1opt;
				mean2=mean2opt;
				std1=std1opt;
				std2=std2opt;
				sum2diff1=0;
				sum2diff2=0;
				sumGauss1=0;
				sumGauss2=0;
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
					gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
					sumGauss1+=gauss1hist[j-min_val];
					sumGauss2+=gauss2hist[j-min_val];
					}
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]*=n1/sumGauss1;
					gauss2hist[j-min_val]*=n2/sumGauss2;
					gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
					sum2diff1+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
					}
				n1=n1opt+dN1;
				sumGauss1=0;
				sumGauss2=0;
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
					gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
					sumGauss1+=gauss1hist[j-min_val];
					sumGauss2+=gauss2hist[j-min_val];
					}
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]*=n1/sumGauss1;
					gauss2hist[j-min_val]*=n2/sumGauss2;
					gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
					sum2diff2+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
					}
				if(sum2diff1<sum2diffMin)
					{
					if(sum2diff1<sum2diff2)
						{
						sum2diffMin=sum2diff1;
						n1opt-=dN1;
						}
					else
						{
						sum2diffMin=sum2diff2;
						n1opt+=dN1;
						}
					}
				else if(sum2diff2<sum2diffMin)
					{
					sum2diffMin=sum2diff2;
					n1opt+=dN1;
					} 
				else
					{
					dN1*=decrease;
					}
				}
	
			if(dN2>=minDN2)
				{
				//Test n2
				n1=n1opt;
				n2=n2opt-dN2;
				mean1=mean1opt;
				mean2=mean2opt;
				std1=std1opt;
				std2=std2opt;
				sum2diff1=0;
				sum2diff2=0;
				sumGauss1=0;
				sumGauss2=0;
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
					gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
					sumGauss1+=gauss1hist[j-min_val];
					sumGauss2+=gauss2hist[j-min_val];
					}
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]*=n1/sumGauss1;
					gauss2hist[j-min_val]*=n2/sumGauss2;
					gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
					sum2diff1+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
					}
				n2=n2opt+dN2;
				sumGauss1=0;
				sumGauss2=0;
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
					gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
					sumGauss1+=gauss1hist[j-min_val];
					sumGauss2+=gauss2hist[j-min_val];
					}
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]*=n1/sumGauss1;
					gauss2hist[j-min_val]*=n2/sumGauss2;
					gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
					sum2diff2+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
					}
				if(sum2diff1<sum2diffMin)
					{
					if(sum2diff1<sum2diff2)
						{
						sum2diffMin=sum2diff1;
						n2opt-=dN2;
						}
					else
						{
						sum2diffMin=sum2diff2;
						n2opt+=dN2;
						}
					}
				else if(sum2diff2<sum2diffMin)
					{
					sum2diffMin=sum2diff2;
					n2opt+=dN2;
					} 
				else
					{
					dN2*=decrease;
					}
				}

			if(dM1>=minDM1)
				{
				//Test m1
				n1=n1opt;
				n2=n2opt;
				mean1=mean1opt-dM1;
				mean2=mean2opt;
				std1=std1opt;
				std2=std2opt;
				sum2diff1=0;
				sum2diff2=0;
				sumGauss1=0;
				sumGauss2=0;
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
					gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
					sumGauss1+=gauss1hist[j-min_val];
					sumGauss2+=gauss2hist[j-min_val];
					}
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]*=n1/sumGauss1;
					gauss2hist[j-min_val]*=n2/sumGauss2;
					gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
					sum2diff1+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
					}
				mean1=mean1opt+dM1;
				sumGauss1=0;
				sumGauss2=0;
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
					gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
					sumGauss1+=gauss1hist[j-min_val];
					sumGauss2+=gauss2hist[j-min_val];
					}
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]*=n1/sumGauss1;
					gauss2hist[j-min_val]*=n2/sumGauss2;
					gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
					sum2diff2+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
					}
				if(sum2diff1<sum2diffMin && mean1opt-dM1>min_val)
					{
					if(sum2diff1<sum2diff2)
						{
						sum2diffMin=sum2diff1;
						mean1opt-=dM1;
						}
					else
						{
						sum2diffMin=sum2diff2;
						mean1opt+=dM1;
						}
					}
				else if(sum2diff2<sum2diffMin)
					{
					sum2diffMin=sum2diff2;
					mean1opt+=dM1;
					} 
				else
					{
					dM1*=decrease;
					}
				}

			if(dM2>=minDM2)
				{
				//Test m2
				n1=n1opt;
				n2=n2opt;
				mean1=mean1opt;
				mean2=mean2opt-dM2;
				std1=std1opt;
				std2=std2opt;
				sum2diff1=0;
				sum2diff2=0;
				sumGauss1=0;
				sumGauss2=0;
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
					gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
					sumGauss1+=gauss1hist[j-min_val];
					sumGauss2+=gauss2hist[j-min_val];
					}
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]*=n1/sumGauss1;
					gauss2hist[j-min_val]*=n2/sumGauss2;
					gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
					sum2diff1+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
					}
				mean2=mean2opt+dM2;
				sumGauss1=0;
				sumGauss2=0;
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
					gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
					sumGauss1+=gauss1hist[j-min_val];
					sumGauss2+=gauss2hist[j-min_val];
					}
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]*=n1/sumGauss1;
					gauss2hist[j-min_val]*=n2/sumGauss2;
					gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
					sum2diff2+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
					}
				if(sum2diff1<sum2diffMin && mean2opt-3*std2>min_val)
					{
					if(sum2diff1<sum2diff2)
						{
						sum2diffMin=sum2diff1;
						mean2opt-=dM2;
						}
					else
						{
						sum2diffMin=sum2diff2;
						mean2opt+=dM2;
						}
					}
				else if(sum2diff2<sum2diffMin)
					{
					sum2diffMin=sum2diff2;
					mean2opt+=dM2;
					} 
				else
					{
					dM2*=decrease;
					}
				}

			if(dS1>=minDS1)
				{
				//Test s1
				n1=n1opt;
				n2=n2opt;
				mean1=mean1opt;
				mean2=mean2opt;
				std1=std1opt-dS1;
				std2=std2opt;
				sum2diff1=0;
				sum2diff2=0;
				sumGauss1=0;
				sumGauss2=0;
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
					gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
					sumGauss1+=gauss1hist[j-min_val];
					sumGauss2+=gauss2hist[j-min_val];
					}
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]*=n1/sumGauss1;
					gauss2hist[j-min_val]*=n2/sumGauss2;
					gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
					sum2diff1+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
					}
				std1=std1opt+dS1;
				sumGauss1=0;
				sumGauss2=0;
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
					gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
					sumGauss1+=gauss1hist[j-min_val];
					sumGauss2+=gauss2hist[j-min_val];
					}
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]*=n1/sumGauss1;
					gauss2hist[j-min_val]*=n2/sumGauss2;
					gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
					sum2diff2+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
					}
				if(sum2diff1<sum2diffMin)
					{
					if(sum2diff1<sum2diff2)
						{
						sum2diffMin=sum2diff1;
						std1opt-=dS1;
						}
					else
						{
						sum2diffMin=sum2diff2;
						std1opt+=dS1;
						}
					}
				else if(sum2diff2<sum2diffMin)
					{
					sum2diffMin=sum2diff2;
					std1opt+=dS1;
					} 
				else
					{
					dS1*=decrease;
					}
				}

			if(dS2>=minDS2)
				{
				//Test s2
				n1=n1opt;
				n2=n2opt;
				mean1=mean1opt;
				mean2=mean2opt;
				std1=std1opt;
				std2=std2opt-dS2;
				sum2diff1=0;
				sum2diff2=0;
				sumGauss1=0;
				sumGauss2=0;
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
					gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
					sumGauss1+=gauss1hist[j-min_val];
					sumGauss2+=gauss2hist[j-min_val];
					}
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]*=n1/sumGauss1;
					gauss2hist[j-min_val]*=n2/sumGauss2;
					gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
					sum2diff1+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
					}
				std2=std2opt+dS2;
				sumGauss1=0;
				sumGauss2=0;
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]=exp(-(j-mean1)*(j-mean1)/(2*std1*std1))/(std1*sqrt2pi);
					gauss2hist[j-min_val]=exp(-(j-mean2)*(j-mean2)/(2*std2*std2))/(std2*sqrt2pi);
					sumGauss1+=gauss1hist[j-min_val];
					sumGauss2+=gauss2hist[j-min_val];
					}
				for(j=min_val; j<=max_val; j++)
					{
					gauss1hist[j-min_val]*=n1/sumGauss1;
					gauss2hist[j-min_val]*=n2/sumGauss2;
					gausstothist[j-min_val]=gauss1hist[j-min_val]+gauss2hist[j-min_val];
					sum2diff2+=(gausstothist[j-min_val]-hist[j-min_val])*(gausstothist[j-min_val]-hist[j-min_val]);
					}
				if(sum2diff1<sum2diffMin/* && mean2opt-3*std2>min_val*/)
					{
					if(sum2diff1<sum2diff2)
						{
						sum2diffMin=sum2diff1;
						std2opt-=dS2;
						}
					else
						{
						sum2diffMin=sum2diff2;
						std2opt+=dS2;
						}
					}
				else if(sum2diff2<sum2diffMin && mean2opt-3*std2>min_val)
					{
					sum2diffMin=sum2diff2;
					std2opt+=dS2;
					} 
				else
					{
					dS2*=decrease;
					}
				}
			}
		}

	double maxGauss=0;
	double cumsum1=0;
	sum1=0;
	sum2=0;
	for(j=min_val; j<=max_val; j++)
		{
		if(gausstothist[j-min_val]>maxGauss)
			maxGauss=gausstothist[j-min_val];
		cumsum1+=gauss2hist[j-min_val];
		
		}
	double err;
	double errMin=cumsum1;
	err_min_ind=max_val;
	for(j=min_val; j<=max_val; j++)
		{
		sum1+=gauss1hist[j-min_val];
		sum2+=gauss2hist[j-min_val];
		err=sum2+cumsum1-sum1;
		if(err<errMin)
			{
			errMin=err;
			err_min_ind=j+1;
			}
		}
	delete[] hist;
	delete[] gauss1hist;
	delete[] gauss2hist;
	delete[] gausstothist;
	return err_min_ind;
	}

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_integer<ELEMTYPE, IMAGEDIM>::otsu(image_binary<IMAGEDIM>* mask)
    {
	this->min_max_refresh();
	ELEMTYPE min_val=this->get_min();
	ELEMTYPE max_val=this->get_max();
	ELEMTYPE err_min_ind=max_val;
	double* hist=new double[1+max_val-min_val];
	memset(hist, 0, sizeof(double)*(1+max_val-min_val));

    typename image_storage<ELEMTYPE >::iterator i = this->begin();
	if (mask!=NULL) 
		{
		typename image_storage<IMGBINARYTYPE >::iterator i2 = mask->begin();
    
		while (i != this->end()) //images are same size and should necessarily end at the same time
			{
			if (*i2) 
				hist[*i-min_val]++;
			++i;
			++i2;
			}
		}
	else 
		{
		while (i != this->end())
			{
			hist[*i-min_val]++;
			++i;
			}
		}

	//Test all threshold values
	double sum1=0;
	double sum2=0;
	double n1=0;
	double n2=0;
	double cumSum=0;
	double cumSum_2=0;
	double cumN=0;

	int j;
	for(j=min_val; j<=max_val; j++)
		{
		cumSum+=j*hist[j-min_val];
		cumSum_2+=j*j*hist[j-min_val];
		cumN+=hist[j-min_val];
		}
	double maxvar=0;
	ELEMTYPE optthr=0;
	for(j=min_val; j<=max_val; j++)
		{
		//Compute mean and std
		sum1+=j*hist[j-min_val];
		n1+=hist[j-min_val];
		sum2=cumSum-sum1;
		n2=cumN-n1;		

		if(n1>0 && n2>0)
			{
			double mean1=sum1/n1;
			double mean2=sum2/n2;
			double betweenvar=n1*n2*(mean1-mean2)*(mean1-mean2)/(n1+n2);
			if(betweenvar>maxvar)
				{
				maxvar=betweenvar;
				optthr=j;
				}
			}
		}
	
	delete[] hist;
	return optthr;
	}

template <class ELEMTYPE, int IMAGEDIM>
std::vector<ELEMTYPE> image_integer<ELEMTYPE, IMAGEDIM>::k_means(int n_means)
    {
    std::vector<ELEMTYPE> res;
	this->min_max_refresh();
	ELEMTYPE min_val=this->get_min();
	ELEMTYPE max_val=this->get_max();
	double* hist=new double[1+max_val-min_val];
	memset(hist, 0, sizeof(double)*(1+max_val-min_val));

    typename image_storage<ELEMTYPE >::iterator iter = this->begin();
    
    while (iter != this->end()) //images are same size and should necessarily end at the same time
        {
        hist[*iter-min_val]++;
        ++iter;
        }

	// K-Means variables
	double* sums=new double[n_means];
	double* counts=new double[n_means];
	double* means=new double[n_means];
	double* thresholds=new double[n_means+1];
	double* newThresholds=new double[n_means+1];	
	
	//init thresholds
	int usedLevels=0;
	int i;
	int j;
	for(j=min_val; j<=max_val; j++)
	{
		if(hist[j-min_val]>0)
			usedLevels++;
	}
	thresholds[0]=min_val;
	double tempsum;
	j=min_val;
	for(i=1; i<n_means; i++)
	{
		tempsum=0;
		while(tempsum<usedLevels/n_means && j<max_val)
		{
			if(hist[j-min_val]>0)
				tempsum++;
			j++;
		}
		thresholds[i]=j;
	}
	thresholds[n_means]=max_val+1;
	int it=0;
	int maxIt=100000;
	bool changed=true;
	while(changed && it<maxIt)
		{
		changed=false;
		for(i=0; i<n_means; i++)
			{	
			sums[i]=0;
			counts[i]=0;
			for(j=thresholds[i]; j<thresholds[i+1]; j++)
				{
				counts[i]+=hist[j-min_val];
				sums[i]+=hist[j-min_val]*j;
				}
			if(counts[i]>0)	
				means[i]=(sums[i]/counts[i]);
			else
				means[i]=thresholds[i];
			}
	
		newThresholds[0]=min_val;
		for(i=1; i<n_means; i++)
			{
			newThresholds[i]=(means[i-1]+means[i])/2;
			}
		newThresholds[n_means]=max_val+1;
		for(i=1; i<n_means; i++)
			{
			if(newThresholds[i]!=thresholds[i])
				changed=true;
			thresholds[i]=newThresholds[i];
			}
		it++;
		}
	for(i=1; i<n_means; i++)
		res.push_back((ELEMTYPE)thresholds[i]);
	delete[] hist;
	return res;
	}

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_integer<ELEMTYPE, IMAGEDIM>::components_hist_3D()
    {
	this->min_max_refresh();
	ELEMTYPE min_val=this->get_min();
	ELEMTYPE max_val=this->get_max();
	ELEMTYPE err_min_ind=min_val;
	int x,y,z,x0,y0,z0,x2,y2,z2;
	int max_x, max_y, max_z;
	max_x=this->get_size_by_dim(0);
	max_y=this->get_size_by_dim(1);
	max_z=this->get_size_by_dim(2);
	int number_of_voxels=max_x*max_y*max_z;
		
	//Sort points
	// create a counting array, counts, with a member for 
    // each possible discrete value in the input.  
    // initialize all counts to 0.
    int distinct_element_count = max_val - min_val + 1;
    int* counts = new int[distinct_element_count];
	memset(counts, 0, sizeof(int)*(1+max_val-min_val));
    // for each value in the unsorted array, increment the
    // count in the corresponding element of the count array
    typename image_storage<ELEMTYPE >::iterator iter = this->begin();    
    while (iter != this->end()) //images are same size and should necessarily end at the same time
        {
        counts[*iter-min_val]++;
        ++iter;
        }
    int counts0=counts[0];
    // accumulate the counts - the result is that counts will hold
    // the offset into the sorted array for the value associated with that index
	int i,j,k;
    for (j=1; j<distinct_element_count; j++)
		{
        counts[j] += counts[j-1];
		}
    // store the elements in a new ordered array
    int* sorted_index = new int[number_of_voxels];
	ELEMTYPE cur_voxel;
	int rest;
	for (i=number_of_voxels-1, iter=this->begin(); i>=0; i--)
		{
            // decrementing the counts value ensures duplicate values in A
            // are stored at different indices in sorted.
			
			sorted_index[--counts[(*(iter+i))-min_val]] = i;                
        }

    int* par_node = new int[number_of_voxels];
    //Init nodes
    for (j=0; j<number_of_voxels; j++)
		{
        par_node[j]=j;
		}
        
    //Search in decreasing order
    int cur_node;
    int adj_node;
	iter = this->end();
    for (i=number_of_voxels-1; i>=counts0; i--)
		{
        j=sorted_index[i];	 
        cur_node=j;
        
        z=j/(max_x*max_y);
        rest=j-z*(max_x*max_y);
        y=rest/max_x;
        x=rest-y*max_x;
		cur_voxel=this->get_voxel(x,y,z);

        z0=z-1;
        z2=z+1;
        y0=y-1;
        y2=y+1;
        x0=x-1;
        x2=x+1;

        //Later neigbours x2,y2,z2
        if(z2<max_z)
			{
			if(this->get_voxel(x,y,z2)>=cur_voxel)
				{
        		k=x+max_x*(y+z2*max_y);
	        	adj_node=findNode(k, par_node);
	        	if(cur_node!=adj_node)
	        		cur_node=mergeNodes(adj_node,cur_node, par_node);
				}
			}
        if(y2<max_y)
			{
			if(this->get_voxel(x,y2,z)>=cur_voxel)
				{
        		k=x+max_x*(y2+z*max_y);
	        	adj_node=findNode(k, par_node);
	        	if(cur_node!=adj_node)
	        		cur_node=mergeNodes(adj_node,cur_node, par_node);
				}
			}
        if(x2<max_x)
			{
			if(this->get_voxel(x2,y,z)>=cur_voxel)
				{
        		k=x2+max_x*(y+z*max_y);
	        	adj_node=findNode(k, par_node);
	        	if(cur_node!=adj_node)
	        		cur_node=mergeNodes(adj_node,cur_node, par_node);
				}
			}
        //Earlier neighbours x0,y0,z0
        if(z0>=0)
			{
			if(this->get_voxel(x,y,z0)>cur_voxel)
				{
        		k=x+max_x*(y+z0*max_y);
	        	adj_node=findNode(k, par_node);
	        	if(cur_node!=adj_node)
	        		cur_node=mergeNodes(adj_node,cur_node, par_node);
				}
			}
        if(y0>=0)
			{
			if(this->get_voxel(x,y0,z)>cur_voxel)
				{
        		k=x+max_x*(y0+z*max_y);
	        	adj_node=findNode(k, par_node);
	        	if(cur_node!=adj_node)
	        		cur_node=mergeNodes(adj_node,cur_node, par_node);
				}
			}
        if(x0>=0)
			{
			if(this->get_voxel(x0,y,z)>cur_voxel)
				{
        		k=x0+max_x*(y+z*max_y);
	        	adj_node=findNode(k, par_node);
	        	if(cur_node!=adj_node)
	        		cur_node=mergeNodes(adj_node,cur_node, par_node);
				}
			}
		}
    double* x_values = new double[1+max_val-min_val];
    double* y_values = new double[1+max_val-min_val];
    double* diff = new double[1+max_val-min_val];
	memset(diff, 0, sizeof(double)*(1+max_val-min_val));	 
	long cumulative_number_of_objects=0;
	double max=0;

	for (i=0, iter=this->begin(); i<number_of_voxels; i++)
		{
        diff[(*(iter+i))-min_val]++;
        diff[(*(iter+par_node[i]))-min_val]--;
		}
	int maxind=0;
    for(i=max_val; i>=min_val; i--)
		{
        cumulative_number_of_objects+=diff[i];
        y_values[i-min_val]=cumulative_number_of_objects;
        x_values[i-min_val]=i;
        if(cumulative_number_of_objects>max)
			{
        	max=cumulative_number_of_objects;
			maxind=i;
			}
		}
        
    int min1ind=min_val;
    double max1;
    double min1;
    double dmax=0;
	int dmax1ind=0;
    i=maxind+1;
    while(i<max_val)
		{
	    //find min
	    while(y_values[i-min_val]<=y_values[i-1-min_val] && i<max_val)
	        i++;
	    min1=y_values[i-1-min_val];
	    min1ind=i-1;
        //find following max
	    while(y_values[i-min_val]>=y_values[i-1-min_val] && i<max_val)
	        i++;
	    max1=y_values[i-1-min_val];
	    if(max1-min1>dmax)
			{
	        dmax=max1-min1;
        	dmax1ind=min1ind;	        	
			}
		}
	i=maxind+1;
	min1=y_values[dmax1ind-min_val];
	min1ind=dmax1ind;
    while(i<dmax1ind)
		{
		if(y_values[i-min_val]<min1)
			{
			min1=y_values[i-min_val];
			min1ind=i;
			}
	    i++;
		}
	err_min_ind=min1ind;
	delete[] counts;
	delete[] sorted_index;
	delete[] par_node;
	delete[] x_values;
	delete[] y_values;
	delete[] diff;

	return err_min_ind;
	}

template <class ELEMTYPE, int IMAGEDIM>
image_label<IMAGEDIM> * image_integer<ELEMTYPE, IMAGEDIM>::narrowest_passage_3D(image_binary<IMAGEDIM> * mask, IMGBINARYTYPE object_value, bool regionGrow)
    {
	this->min_max_refresh();
    image_label<IMAGEDIM> * output = new image_label<IMAGEDIM> (this,false);
	IMGLABELTYPE class1=3;
	IMGLABELTYPE class2=4;
	IMGLABELTYPE undef=1;
	IMGLABELTYPE conflict=2;
	IMGLABELTYPE bkg=0;
	ELEMTYPE min_val=this->get_min();
	ELEMTYPE max_val=this->get_max();
	ELEMTYPE err_min_ind=min_val;
	int x,y,z,x0,y0,z0,x2,y2,z2;
	int max_x, max_y, max_z;
	max_x=this->get_size_by_dim(0);
	max_y=this->get_size_by_dim(1);
	max_z=this->get_size_by_dim(2);
	int number_of_voxels=this->num_elements;//max_x*max_y*max_z;
	output->erase();
		
	//Sort points
	// create a counting array, counts, with a member for 
    // each possible discrete value in the input.  
    // initialize all counts to 0.
    int distinct_element_count = max_val - min_val + 1;
    int* counts = new int[distinct_element_count];
	memset(counts, 0, sizeof(int)*(1+max_val-min_val));
    int* counts_cum = new int[distinct_element_count];
	memset(counts_cum, 0, sizeof(int)*(1+max_val-min_val));
    // for each value in the unsorted array, increment the
    // count in the corresponding element of the count array
    typename image_storage<ELEMTYPE >::iterator iter = this->begin(); 
	typename image_storage<IMGLABELTYPE >::iterator output_iter = output->begin();   
    while (iter != this->end()) //images are same size and should necessarily end at the same time
        {
        counts[(*iter)-min_val]++;
        ++iter;
        }
    counts_cum[0]=counts[0];
    // accumulate the counts - the result is that counts will hold
    // the offset into the sorted array for the value associated with that index
	int i,j,k;
    for (j=1; j<distinct_element_count; j++)
		{
        counts_cum[j] = counts_cum[j-1]+counts[j];
		}
    // store the elements in a new ordered array
    int* sorted_index = new int[number_of_voxels];
	ELEMTYPE cur_voxel;
	int rest;
	for (i=number_of_voxels-1, iter=this->begin(); i>=0; i--)
		{
            // decrementing the counts value ensures duplicate values in A
            // are stored at different indices in sorted.
			
			sorted_index[--counts_cum[(*(iter+i))-min_val]] = i;                
        }

    int* par_node = new int[number_of_voxels];
    //Init nodes
    for (j=0; j<number_of_voxels; j++)
		{
        par_node[j]=j;
		}
        
    //Search in decreasing order
    int cur_node;
    int adj_node;
	iter = this->end();
    for (i=number_of_voxels-1; i>=counts[0]; i--)
		{
        j=sorted_index[i];	 
        cur_node=j;
		(*(output_iter+j))=undef;
        
        z=j/(max_x*max_y);
        rest=j-z*(max_x*max_y);
        y=rest/max_x;
        x=rest-y*max_x;

		cur_voxel=this->get_voxel(x,y,z);

        z0=z-1;
        z2=z+1;
        y0=y-1;
        y2=y+1;
        x0=x-1;
        x2=x+1;

        //Later neigbours x2,y2,z2
        if(z2<max_z)
			{
			if(this->get_voxel(x,y,z2)>=cur_voxel)
				{
        		k=x+max_x*(y+z2*max_y);
	        	adj_node=findNode(k, par_node);
	        	if(cur_node!=adj_node)
	        		cur_node=mergeNodes(adj_node,cur_node, par_node);
				}
			}
        if(y2<max_y)
			{
			if(this->get_voxel(x,y2,z)>=cur_voxel)
				{
        		k=x+max_x*(y2+z*max_y);
	        	adj_node=findNode(k, par_node);
	        	if(cur_node!=adj_node)
	        		cur_node=mergeNodes(adj_node,cur_node, par_node);
				}
			}
        if(x2<max_x)
			{
			if(this->get_voxel(x2,y,z)>=cur_voxel)
				{
        		k=x2+max_x*(y+z*max_y);
	        	adj_node=findNode(k, par_node);
	        	if(cur_node!=adj_node)
	        		cur_node=mergeNodes(adj_node,cur_node, par_node);
				}
			}
        //Earlier neighbours x0,y0,z0
        if(z0>=0)
			{
			if(this->get_voxel(x,y,z0)>cur_voxel)
				{
        		k=x+max_x*(y+z0*max_y);
	        	adj_node=findNode(k, par_node);
	        	if(cur_node!=adj_node)
	        		cur_node=mergeNodes(adj_node,cur_node, par_node);
				}
			}
        if(y0>=0)
			{
			if(this->get_voxel(x,y0,z)>cur_voxel)
				{
        		k=x+max_x*(y0+z*max_y);
	        	adj_node=findNode(k, par_node);
	        	if(cur_node!=adj_node)
	        		cur_node=mergeNodes(adj_node,cur_node, par_node);
				}
			}
        if(x0>=0)
			{
			if(this->get_voxel(x0,y,z)>cur_voxel)
				{
        		k=x0+max_x*(y+z*max_y);
	        	adj_node=findNode(k, par_node);
	        	if(cur_node!=adj_node)
	        		cur_node=mergeNodes(adj_node,cur_node, par_node);
				}
			}
		}

	//Find seeds
	bool* marked=new bool[number_of_voxels];
	memset(marked, 0, sizeof(bool)*number_of_voxels);
	typename image_storage<IMGBINARYTYPE >::iterator mask_iter = mask->begin(); 
	output_iter = output->begin(); 
	for (i=0; i<number_of_voxels; i++)
		{
		if((*(mask_iter+i))==object_value && (*(output_iter+i))!=bkg)
			{
			markRecursive(i,par_node,marked);
			(*(output_iter+i))=class1;
			}
		}

	//NPT image
	ELEMTYPE* npt_array = new ELEMTYPE[number_of_voxels];
	memset(npt_array, 0, sizeof(ELEMTYPE)*number_of_voxels);
	for (i=number_of_voxels-1; i>=counts[0]; i--)//Skip lowest value
		{
	    j=sorted_index[i];
	    npt_array[j]=getSeedLevel(j, par_node, marked);
		}
	    
	//Equal neighbour
	bool* eq_neigh=new bool[number_of_voxels];
	int n_diff;
	for (i=number_of_voxels-1; i>=counts[0]; i--)//Skip lowest value
		{
	    j=sorted_index[i];
	    n_diff=0;
	    
	    z=j/(max_x*max_y);
	    rest=j-z*(max_x*max_y);
	    y=rest/max_x;
	    x=rest-y*max_x;

		for(z2=std::max(0,z-1); z2<std::min(max_z,z+2) && n_diff==0; z2++)
			{
			for(y2=std::max(0,y-1); y2<std::min(max_y,y+2) && n_diff==0; y2++)
				{
				for(x2=std::max(0,x-1); x2<std::min(max_x,x+2) && n_diff==0; x2++)
					{
			        k=x2+max_x*(y2+z2*max_y);
				    if(npt_array[k]!=npt_array[j])
				        {
				        n_diff++;
				        }										
					}
				}
			}
	  //  if(n_diff==0)
	  //      {
			//(*(output_iter+j))=class2;
	  //      }
		if(n_diff==0)
			{
			//Test directions
			bool hitTarget=false;
			bool hitBkg=false;

			//Left
			x2=x-1;
			while(!hitTarget && !hitBkg && x2>=0)
				{
				k=x2+max_x*(y+z*max_y);
				if((*(output_iter+k))==class1)
					hitTarget=true;
				else if((*(output_iter+k))==bkg)
					hitBkg=true;
				x2--;
				}
			hitBkg=false;

			//Right
			x2=x+1;
			while(!hitTarget && !hitBkg && x2<max_x)
				{
				k=x2+max_x*(y+z*max_y);
				if((*(output_iter+k))==class1)
					hitTarget=true;
				else if((*(output_iter+k))==bkg)
					hitBkg=true;
				x2++;
				}
			hitBkg=false;

			//Up
			y2=y-1;
			while(!hitTarget && !hitBkg && y2>=0)
				{
				k=x+max_x*(y2+z*max_y);
				if((*(output_iter+k))==class1)
					hitTarget=true;
				else if((*(output_iter+k))==bkg)
					hitBkg=true;
				y2--;
				}
			hitBkg=false;

			//Down
			y2=y+1;
			while(!hitTarget && !hitBkg && y2<max_y)
				{
				k=x+max_x*(y2+z*max_y);
				if((*(output_iter+k))==class1)
					hitTarget=true;
				else if((*(output_iter+k))==bkg)
					hitBkg=true;
				y2++;
				}
			hitBkg=false;

			//Above
			z2=z-1;
			while(!hitTarget && !hitBkg && z2>=0)
				{
				k=x+max_x*(y+z2*max_y);
				if((*(output_iter+k))==class1)
					hitTarget=true;
				else if((*(output_iter+k))==bkg)
					hitBkg=true;
				z2--;
				}
			hitBkg=false;

			//Below
			z2=z+1;
			while(!hitTarget && !hitBkg && z2<max_z)
				{
				k=x+max_x*(y+z2*max_y);
				if((*(output_iter+k))==class1)
					hitTarget=true;
				else if((*(output_iter+k))==bkg)
					hitBkg=true;
				z2++;
				}
			hitBkg=false;

			if(!hitTarget)
				(*(output_iter+j))=class2;
			}
		}


	if(regionGrow)
		{
		//Region Grow
		ELEMTYPE cur_level;
		int nClass1, nClass2;
		bool changed=true;
		bool changedThis;
		while(changed)
			{
			changed=false;
			for(cur_level=max_val; cur_level>min_val; cur_level--)
				{
				//Loop each level until not changed
				changedThis=true;
				while(changedThis)
					{
					changedThis=false;
					for (i=counts_cum[cur_level-min_val]-1; i>=counts_cum[cur_level-1-min_val]; i--)
						{
						j=sorted_index[i];
						if((*(output_iter+j))==undef)
							{
							//Check neighbours
							z=j/(max_x*max_y);
							rest=j-z*(max_x*max_y);
							y=rest/max_x;
							x=rest-y*max_x;

							nClass1=nClass2=0;

							for(z2=std::max(0,z-1); z2<std::min(max_z,z+2); z2++)
								{
								for(y2=std::max(0,y-1); y2<std::min(max_y,y+2); y2++)
									{
									for(x2=std::max(0,x-1); x2<std::min(max_x,x+2); x2++)
										{
										k=x2+max_x*(y2+z2*max_y);
										if((*(output_iter+k))==class1)
											nClass1++;
										else if((*(output_iter+k))==class2)
											nClass2++;										
										}
									}
								}
							if(nClass1>0 && nClass2==0)
								{
								(*(output_iter+j))=class1;
								changed=true;
								changedThis=true;
								}
							else if(nClass2>0 && nClass1==0)
								{
								(*(output_iter+j))=class2;
								changed=true;
								changedThis=true;
								}
							else if(nClass2>0 && nClass1>0)
								{
								(*(output_iter+j))=conflict;
								changed=true;
								changedThis=true;
								}
							}
						}
					}
				}		        
			} 
		//Clean up undefined
		for (i=number_of_voxels-1; i>=counts[0]; i--)//Skip lowest value
			{
			j=sorted_index[i];
			if((*(output_iter+j))==undef)
				{			   
				(*(output_iter+j))=class2;  
				}
			} 
		}
	delete[] counts;
	delete[] counts_cum;
	delete[] sorted_index;
	delete[] par_node;
	delete[] marked;
	delete[] npt_array;
	delete[] eq_neigh;

	//output->min_max_refresh();
	return output;
	}

template <class ELEMTYPE, int IMAGEDIM>
image_label<IMAGEDIM> * image_integer<ELEMTYPE, IMAGEDIM>::narrowest_passage_3D(image_binary<IMAGEDIM> * mask, int low_x, int high_x, int low_y, int high_y, int low_z, int high_z, IMGBINARYTYPE object_value, bool regionGrow)
    {
	this->min_max_refresh();
    image_label<IMAGEDIM> * output = new image_label<IMAGEDIM> (this,false);
	IMGLABELTYPE class1=3;
	IMGLABELTYPE class2=4;
	IMGLABELTYPE undef=1;
	IMGLABELTYPE conflict=2;
	IMGLABELTYPE bkg=0;

	ELEMTYPE min_val=this->get_min();
	ELEMTYPE max_val=this->get_max();
	ELEMTYPE err_min_ind=min_val;
	int x,y,z,x0,y0,z0,x2,y2,z2;
	int max_x, max_y, max_z;
	max_x=this->get_size_by_dim(0);
	max_y=this->get_size_by_dim(1);
	max_z=this->get_size_by_dim(2);
	if(low_x<0)
		low_x=0;
	if(low_y<0)
		low_y=0;
	if(low_z<0)
		low_z=0;
	if(high_x<0 || high_x>max_x)
		high_x=max_x;
	if(high_y<0 || high_y>max_y)
		high_y=max_y;
	if(high_z<0 || high_z>max_z)
		high_z=max_z;

	int number_of_voxels=this->num_elements;//max_x*max_y*max_z;
	output->erase();
		
	//Sort points
	// create a counting array, counts, with a member for 
    // each possible discrete value in the input.  
    // initialize all counts to 0.
    int distinct_element_count = max_val - min_val + 1;
    int* counts = new int[distinct_element_count];
	memset(counts, 0, sizeof(int)*(1+max_val-min_val));
    int* counts_cum = new int[distinct_element_count];
	memset(counts_cum, 0, sizeof(int)*(1+max_val-min_val));
    // for each value in the unsorted array, increment the
    // count in the corresponding element of the count array
    typename image_storage<ELEMTYPE >::iterator iter = this->begin(); 
	typename image_storage<IMGLABELTYPE >::iterator output_iter = output->begin();   
    while (iter != this->end()) //images are same size and should necessarily end at the same time
        {
        counts[(*iter)-min_val]++;
        ++iter;
        }
    counts_cum[0]=counts[0];
    // accumulate the counts - the result is that counts will hold
    // the offset into the sorted array for the value associated with that index
	int i,j,k;
    for (j=1; j<distinct_element_count; j++)
		{
        counts_cum[j] = counts_cum[j-1]+counts[j];
		}
    // store the elements in a new ordered array
    int* sorted_index = new int[number_of_voxels];
	ELEMTYPE cur_voxel;
	int rest;
	for (i=number_of_voxels-1, iter=this->begin(); i>=0; i--)
		{
            // decrementing the counts value ensures duplicate values in A
            // are stored at different indices in sorted.
			
			sorted_index[--counts_cum[(*(iter+i))-min_val]] = i;                
        }

    int* par_node = new int[number_of_voxels];
    //Init nodes
    for (j=0; j<number_of_voxels; j++)
		{
        par_node[j]=j;
		}
        
    //Search in decreasing order
    int cur_node;
    int adj_node;
	iter = this->end();
    for (i=number_of_voxels-1; i>=counts[0]; i--)
		{
        j=sorted_index[i];	 
        cur_node=j;
		(*(output_iter+j))=undef;
        
        z=j/(max_x*max_y);
        rest=j-z*(max_x*max_y);
        y=rest/max_x;
        x=rest-y*max_x;

		if(x>=low_x && x<high_x && y>=low_y && y<high_y && z>=low_z && z<high_z) 
			{
			cur_voxel=this->get_voxel(x,y,z);

			z0=z-1;
			z2=z+1;
			y0=y-1;
			y2=y+1;
			x0=x-1;
			x2=x+1;

			//Later neigbours x2,y2,z2
			if(z2<high_z)
				{
				if(this->get_voxel(x,y,z2)>=cur_voxel)
					{
        			k=x+max_x*(y+z2*max_y);
	        		adj_node=findNode(k, par_node);
	        		if(cur_node!=adj_node)
	        			cur_node=mergeNodes(adj_node,cur_node, par_node);
					}
				}
			if(y2<high_y)
				{
				if(this->get_voxel(x,y2,z)>=cur_voxel)
					{
        			k=x+max_x*(y2+z*max_y);
	        		adj_node=findNode(k, par_node);
	        		if(cur_node!=adj_node)
	        			cur_node=mergeNodes(adj_node,cur_node, par_node);
					}
				}
			if(x2<high_x)
				{
				if(this->get_voxel(x2,y,z)>=cur_voxel)
					{
        			k=x2+max_x*(y+z*max_y);
	        		adj_node=findNode(k, par_node);
	        		if(cur_node!=adj_node)
	        			cur_node=mergeNodes(adj_node,cur_node, par_node);
					}
				}
			//Earlier neighbours x0,y0,z0
			if(z0>=low_z)
				{
				if(this->get_voxel(x,y,z0)>cur_voxel)
					{
        			k=x+max_x*(y+z0*max_y);
	        		adj_node=findNode(k, par_node);
	        		if(cur_node!=adj_node)
	        			cur_node=mergeNodes(adj_node,cur_node, par_node);
					}
				}
			if(y0>=low_y)
				{
				if(this->get_voxel(x,y0,z)>cur_voxel)
					{
        			k=x+max_x*(y0+z*max_y);
	        		adj_node=findNode(k, par_node);
	        		if(cur_node!=adj_node)
	        			cur_node=mergeNodes(adj_node,cur_node, par_node);
					}
				}
			if(x0>=low_x)
				{
				if(this->get_voxel(x0,y,z)>cur_voxel)
					{
        			k=x0+max_x*(y+z*max_y);
	        		adj_node=findNode(k, par_node);
	        		if(cur_node!=adj_node)
	        			cur_node=mergeNodes(adj_node,cur_node, par_node);
					}
				}
			}
		}

	//Find seeds
	bool* marked=new bool[number_of_voxels];
	memset(marked, 0, sizeof(bool)*number_of_voxels);
	typename image_storage<IMGBINARYTYPE >::iterator mask_iter = mask->begin(); 
	output_iter = output->begin(); 
	for (i=0; i<number_of_voxels; i++)
		{
		if((*(mask_iter+i))==object_value && (*(output_iter+i))!=bkg)
			{
			markRecursive(i,par_node,marked);
			(*(output_iter+i))=class1;
			}
		}

	//NPT image
	ELEMTYPE* npt_array = new ELEMTYPE[number_of_voxels];
	memset(npt_array, 0, sizeof(ELEMTYPE)*number_of_voxels);
	for (i=number_of_voxels-1; i>=counts[0]; i--)//Skip lowest value
		{
	    j=sorted_index[i];
	    npt_array[j]=getSeedLevel(j, par_node, marked);
		}
	    
	//Equal neighbour
	bool* eq_neigh=new bool[number_of_voxels];
	int n_diff;
	for (i=number_of_voxels-1; i>=counts[0]; i--)//Skip lowest value
		{
	    j=sorted_index[i];
	    n_diff=0;
	    
	    z=j/(max_x*max_y);
	    rest=j-z*(max_x*max_y);
	    y=rest/max_x;
	    x=rest-y*max_x;

		if(x>=low_x && x<high_x && y>=low_y && y<high_y && z>=low_z && z<high_z) 
			{
			for(z2=std::max(low_z,z-1); z2<std::min(high_z,z+2) && n_diff==0; z2++)
				{
				for(y2=std::max(low_y,y-1); y2<std::min(high_y,y+2) && n_diff==0; y2++)
					{
					for(x2=std::max(low_x,x-1); x2<std::min(high_x,x+2) && n_diff==0; x2++)
						{
						k=x2+max_x*(y2+z2*max_y);
						if(npt_array[k]!=npt_array[j])
							{
							n_diff++;
							}										
						}
					}
				}
			if(n_diff==0)
				{
				//Test directions
				bool hitTarget=false;
				bool hitBkg=false;

				//Left
				x2=x-1;
				while(!hitTarget && !hitBkg && x2>=low_x)
					{
					k=x2+max_x*(y+z*max_y);
					if((*(output_iter+k))==class1)
						hitTarget=true;
					else if((*(output_iter+k))==bkg)
						hitBkg=true;
					x2--;
					}
				hitBkg=false;

				//Right
				x2=x+1;
				while(!hitTarget && !hitBkg && x2<high_x)
					{
					k=x2+max_x*(y+z*max_y);
					if((*(output_iter+k))==class1)
						hitTarget=true;
					else if((*(output_iter+k))==bkg)
						hitBkg=true;
					x2++;
					}
				hitBkg=false;

				//Up
				y2=y-1;
				while(!hitTarget && !hitBkg && y2>=low_y)
					{
					k=x+max_x*(y2+z*max_y);
					if((*(output_iter+k))==class1)
						hitTarget=true;
					else if((*(output_iter+k))==bkg)
						hitBkg=true;
					y2--;
					}
				hitBkg=false;

				//Down
				y2=y+1;
				while(!hitTarget && !hitBkg && y2<high_y)
					{
					k=x+max_x*(y2+z*max_y);
					if((*(output_iter+k))==class1)
						hitTarget=true;
					else if((*(output_iter+k))==bkg)
						hitBkg=true;
					y2++;
					}
				hitBkg=false;

				//Above
				z2=z-1;
				while(!hitTarget && !hitBkg && z2>=low_z)
					{
					k=x+max_x*(y+z2*max_y);
					if((*(output_iter+k))==class1)
						hitTarget=true;
					else if((*(output_iter+k))==bkg)
						hitBkg=true;
					z2--;
					}
				hitBkg=false;

				//Below
				z2=z+1;
				while(!hitTarget && !hitBkg && z2<high_z)
					{
					k=x+max_x*(y+z2*max_y);
					if((*(output_iter+k))==class1)
						hitTarget=true;
					else if((*(output_iter+k))==bkg)
						hitBkg=true;
					z2++;
					}
				hitBkg=false;

				if(!hitTarget)
					(*(output_iter+j))=class2;
				}
			}
		}

	if(regionGrow)
		{
		//RegionGrow
		ELEMTYPE cur_level;
		int nClass1, nClass2;
		bool changed=true;
		bool changedThis;
		while(changed)
			{
			changed=false;
			for(cur_level=max_val; cur_level>min_val; cur_level--)
				{
				//Loop each level until not changed
				changedThis=true;
				while(changedThis)
					{
					changedThis=false;
					for (i=counts_cum[cur_level-min_val]-1; i>=counts_cum[cur_level-1-min_val]; i--)
						{
						j=sorted_index[i];
						if((*(output_iter+j))==undef)
							{
							//Check neighbours
							z=j/(max_x*max_y);
							rest=j-z*(max_x*max_y);
							y=rest/max_x;
							x=rest-y*max_x;

							//if(x>=low_x && x<high_x && y>=low_y && y<high_y && z>=low_z && z<high_z) 
								{
								nClass1=nClass2=0;

								//for(z2=std::max(low_z,z-1); z2<std::min(high_z,z+2); z2++)
								//	{
								//	for(y2=std::max(low_y,y-1); y2<std::min(high_y,y+2); y2++)
								//		{
								//		for(x2=std::max(low_x,x-1); x2<std::min(high_x,x+2); x2++)
								for(z2=std::max(0,z-1); z2<std::min(max_z,z+2); z2++)
									{
									for(y2=std::max(0,y-1); y2<std::min(max_y,y+2); y2++)
										{
										for(x2=std::max(0,x-1); x2<std::min(max_x,x+2); x2++)
											{
											k=x2+max_x*(y2+z2*max_y);
											if((*(output_iter+k))==class1)
												nClass1++;
											else if((*(output_iter+k))==class2)
												nClass2++;										
											}
										}
									}
								if(nClass1>nClass2)
									{
									(*(output_iter+j))=class1;
									changed=true;
									changedThis=true;
									}
								else if(nClass2>nClass1)
									{
									(*(output_iter+j))=class2;
									changed=true;
									changedThis=true;
									}
								//else if(nClass2>0 && nClass1>0)
								//	{
								//	(*(output_iter+j))=conflict;
								//	changed=true;
								//	changedThis=true;
								//	}
								}
							//else
							//	{
							//	(*(output_iter+j))=class1;
							//	changed=true;
							//	changedThis=true;
							//	}
							}
						}
					}
				}		        
			}
		//Clean up undefined
		for (i=number_of_voxels-1; i>=counts[0]; i--)//Skip lowest value
			{
			j=sorted_index[i];
			if((*(output_iter+j))==undef)
				{			    
				z=j/(max_x*max_y);
				rest=j-z*(max_x*max_y);
				y=rest/max_x;
				x=rest-y*max_x;

				if(x>=low_x && x<high_x && y>=low_y && y<high_y && z>=low_z && z<high_z) 
					(*(output_iter+j))=class2;  
				else
					(*(output_iter+j))=class1;  
				}
			}
		}
	delete[] counts;
	delete[] counts_cum;
	delete[] sorted_index;
	delete[] par_node;
	delete[] marked;
	delete[] npt_array;
	delete[] eq_neigh;

	//output->min_max_refresh();
	return output;
	}

template <class ELEMTYPE, int IMAGEDIM>
image_label<IMAGEDIM> * image_integer<ELEMTYPE, IMAGEDIM>::narrowest_passage_3D(image_binary<IMAGEDIM> * mask, int* no_internal_seeds, int y_direction, int z_direction, bool allow_before,IMGBINARYTYPE object_value, bool regionGrow)
    {
	this->min_max_refresh();
    image_label<IMAGEDIM> * output = new image_label<IMAGEDIM> (this,false);
	IMGLABELTYPE class1=3;
	IMGLABELTYPE class2=4;
	IMGLABELTYPE undef=1;
	IMGLABELTYPE conflict=2;
	IMGLABELTYPE bkg=0;

	ELEMTYPE min_val=this->get_min();
	ELEMTYPE max_val=this->get_max();
	ELEMTYPE err_min_ind=min_val;
	int x,y,z,x0,y0,z0,x2,y2,z2;
	int max_x, max_y, max_z;
	max_x=this->get_size_by_dim(0);
	max_y=this->get_size_by_dim(1);
	max_z=this->get_size_by_dim(2);
	//if(low_x<0)
	//	low_x=0;
	//if(low_y<0)
	//	low_y=0;
	//if(low_z<0)
	//	low_z=0;
	//if(high_x<0 || high_x>max_x)
	//	high_x=max_x;
	//if(high_y<0 || high_y>max_y)
	//	high_y=max_y;
	//if(high_z<0 || high_z>max_z)
	//	high_z=max_z;

	int number_of_voxels=this->num_elements;//max_x*max_y*max_z;
	output->erase();
		
	//Sort points
	// create a counting array, counts, with a member for 
    // each possible discrete value in the input.  
    // initialize all counts to 0.
    int distinct_element_count = max_val - min_val + 1;
    int* counts = new int[distinct_element_count];
	memset(counts, 0, sizeof(int)*(1+max_val-min_val));
    int* counts_cum = new int[distinct_element_count];
	memset(counts_cum, 0, sizeof(int)*(1+max_val-min_val));
    // for each value in the unsorted array, increment the
    // count in the corresponding element of the count array
    typename image_storage<ELEMTYPE >::iterator iter = this->begin(); 
	typename image_storage<IMGLABELTYPE >::iterator output_iter = output->begin();   
    while (iter != this->end()) //images are same size and should necessarily end at the same time
        {
        counts[(*iter)-min_val]++;
        ++iter;
        }
    counts_cum[0]=counts[0];
    // accumulate the counts - the result is that counts will hold
    // the offset into the sorted array for the value associated with that index
	int i,j,k;
    for (j=1; j<distinct_element_count; j++)
		{
        counts_cum[j] = counts_cum[j-1]+counts[j];
		}
    // store the elements in a new ordered array
    int* sorted_index = new int[number_of_voxels];
	ELEMTYPE cur_voxel;
	int rest;
	for (i=number_of_voxels-1, iter=this->begin(); i>=0; i--)
		{
            // decrementing the counts value ensures duplicate values in A
            // are stored at different indices in sorted.
			
			sorted_index[--counts_cum[(*(iter+i))-min_val]] = i;                
        }

    int* par_node = new int[number_of_voxels];
    //Init nodes
    for (j=0; j<number_of_voxels; j++)
		{
        par_node[j]=j;
		}
        
    //Search in decreasing order
    int cur_node;
    int adj_node;
	iter = this->end();
	int dims[3];
    for (i=number_of_voxels-1; i>=counts[0]; i--)
		{
        j=sorted_index[i];	 
        cur_node=j;
		(*(output_iter+j))=undef;
        
        z=j/(max_x*max_y);
        rest=j-z*(max_x*max_y);
        y=rest/max_x;
        x=rest-y*max_x;
		dims[0]=x;
		dims[1]=y;
		dims[2]=z;
		
		//if((allow_before && dims[y_direction]<no_internal_seeds[dims[z_direction]]) || ((!allow_before) && dims[y_direction]>no_internal_seeds[dims[z_direction]]))
		//if(x>=low_x && x<high_x && y>=low_y && y<high_y && z>=low_z && z<high_z) 
			{
			cur_voxel=this->get_voxel(x,y,z);

			z0=z-1;
			z2=z+1;
			y0=y-1;
			y2=y+1;
			x0=x-1;
			x2=x+1;

			//Later neigbours x2,y2,z2
			if(z2<max_z)
				{
				if(this->get_voxel(x,y,z2)>=cur_voxel)
					{
        			k=x+max_x*(y+z2*max_y);
	        		adj_node=findNode(k, par_node);
	        		if(cur_node!=adj_node)
	        			cur_node=mergeNodes(adj_node,cur_node, par_node);
					}
				}
			if(y2<max_y)
				{
				if(this->get_voxel(x,y2,z)>=cur_voxel)
					{
        			k=x+max_x*(y2+z*max_y);
	        		adj_node=findNode(k, par_node);
	        		if(cur_node!=adj_node)
	        			cur_node=mergeNodes(adj_node,cur_node, par_node);
					}
				}
			if(x2<max_x)
				{
				if(this->get_voxel(x2,y,z)>=cur_voxel)
					{
        			k=x2+max_x*(y+z*max_y);
	        		adj_node=findNode(k, par_node);
	        		if(cur_node!=adj_node)
	        			cur_node=mergeNodes(adj_node,cur_node, par_node);
					}
				}
			//Earlier neighbours x0,y0,z0
			if(z0>=0)
				{
				if(this->get_voxel(x,y,z0)>cur_voxel)
					{
        			k=x+max_x*(y+z0*max_y);
	        		adj_node=findNode(k, par_node);
	        		if(cur_node!=adj_node)
	        			cur_node=mergeNodes(adj_node,cur_node, par_node);
					}
				}
			if(y0>=0)
				{
				if(this->get_voxel(x,y0,z)>cur_voxel)
					{
        			k=x+max_x*(y0+z*max_y);
	        		adj_node=findNode(k, par_node);
	        		if(cur_node!=adj_node)
	        			cur_node=mergeNodes(adj_node,cur_node, par_node);
					}
				}
			if(x0>=0)
				{
				if(this->get_voxel(x0,y,z)>cur_voxel)
					{
        			k=x0+max_x*(y+z*max_y);
	        		adj_node=findNode(k, par_node);
	        		if(cur_node!=adj_node)
	        			cur_node=mergeNodes(adj_node,cur_node, par_node);
					}
				}
			}
		}

	//Find seeds
	bool* marked=new bool[number_of_voxels];
	memset(marked, 0, sizeof(bool)*number_of_voxels);
	typename image_storage<IMGBINARYTYPE >::iterator mask_iter = mask->begin(); 
	output_iter = output->begin(); 
	for (i=0; i<number_of_voxels; i++)
		{
		if((*(mask_iter+i))==object_value && (*(output_iter+i))!=bkg)
			{
			markRecursive(i,par_node,marked);
			(*(output_iter+i))=class1;
			}
		}

	//NPT image
	ELEMTYPE* npt_array = new ELEMTYPE[number_of_voxels];
	memset(npt_array, 0, sizeof(ELEMTYPE)*number_of_voxels);
	for (i=number_of_voxels-1; i>=counts[0]; i--)//Skip lowest value
		{
	    j=sorted_index[i];
	    npt_array[j]=getSeedLevel(j, par_node, marked);
		}
	    
	//Equal neighbour
	bool* eq_neigh=new bool[number_of_voxels];
	int n_diff;
	for (i=number_of_voxels-1; i>=counts[0]; i--)//Skip lowest value
		{
	    j=sorted_index[i];
	    n_diff=0;
	    
	    z=j/(max_x*max_y);
	    rest=j-z*(max_x*max_y);
	    y=rest/max_x;
	    x=rest-y*max_x;
		
		dims[0]=x;
		dims[1]=y;
		dims[2]=z;
		
		if((allow_before && dims[y_direction]<no_internal_seeds[dims[z_direction]]) || ((!allow_before) && dims[y_direction]>no_internal_seeds[dims[z_direction]]))
		//if(x>=0 && x<max_x && y>=0 && y<max_y && z>=0 && z<max_z) 
			{
			for(z2=std::max(0,z-1); z2<std::min(max_z,z+2) && n_diff==0; z2++)
				{
				for(y2=std::max(0,y-1); y2<std::min(max_y,y+2) && n_diff==0; y2++)
					{
					for(x2=std::max(0,x-1); x2<std::min(max_x,x+2) && n_diff==0; x2++)
						{
						k=x2+max_x*(y2+z2*max_y);
						if(npt_array[k]!=npt_array[j])
							{
							n_diff++;
							}										
						}
					}
				}
			if(n_diff==0)
				{
				//Test directions
				bool hitTarget=false;
				bool hitBkg=false;

				//Left
				x2=x-1;
				while(!hitTarget && !hitBkg && x2>=0)
					{
					k=x2+max_x*(y+z*max_y);
					if((*(output_iter+k))==class1)
						hitTarget=true;
					else if((*(output_iter+k))==bkg)
						hitBkg=true;
					x2--;
					}
				hitBkg=false;

				//Right
				x2=x+1;
				while(!hitTarget && !hitBkg && x2<max_x)
					{
					k=x2+max_x*(y+z*max_y);
					if((*(output_iter+k))==class1)
						hitTarget=true;
					else if((*(output_iter+k))==bkg)
						hitBkg=true;
					x2++;
					}
				hitBkg=false;

				//Up
				y2=y-1;
				while(!hitTarget && !hitBkg && y2>=0)
					{
					k=x+max_x*(y2+z*max_y);
					if((*(output_iter+k))==class1)
						hitTarget=true;
					else if((*(output_iter+k))==bkg)
						hitBkg=true;
					y2--;
					}
				hitBkg=false;

				//Down
				y2=y+1;
				while(!hitTarget && !hitBkg && y2<max_y)
					{
					k=x+max_x*(y2+z*max_y);
					if((*(output_iter+k))==class1)
						hitTarget=true;
					else if((*(output_iter+k))==bkg)
						hitBkg=true;
					y2++;
					}
				hitBkg=false;

				//Above
				z2=z-1;
				while(!hitTarget && !hitBkg && z2>=0)
					{
					k=x+max_x*(y+z2*max_y);
					if((*(output_iter+k))==class1)
						hitTarget=true;
					else if((*(output_iter+k))==bkg)
						hitBkg=true;
					z2--;
					}
				hitBkg=false;

				//Below
				z2=z+1;
				while(!hitTarget && !hitBkg && z2<max_z)
					{
					k=x+max_x*(y+z2*max_y);
					if((*(output_iter+k))==class1)
						hitTarget=true;
					else if((*(output_iter+k))==bkg)
						hitBkg=true;
					z2++;
					}
				hitBkg=false;

				if(!hitTarget)
					(*(output_iter+j))=class2;
				}
			}
		}

	if(regionGrow)
		{
		//RegionGrow
		ELEMTYPE cur_level;
		int nClass1, nClass2;
		bool changed=true;
		bool changedThis;
		while(changed)
			{
			changed=false;
			for(cur_level=max_val; cur_level>min_val; cur_level--)
				{
				//Loop each level until not changed
				changedThis=true;
				while(changedThis)
					{
					changedThis=false;
					for (i=counts_cum[cur_level-min_val]-1; i>=counts_cum[cur_level-1-min_val]; i--)
						{
						j=sorted_index[i];
						if((*(output_iter+j))==undef)
							{
							//Check neighbours
							z=j/(max_x*max_y);
							rest=j-z*(max_x*max_y);
							y=rest/max_x;
							x=rest-y*max_x;

							//if(x>=low_x && x<high_x && y>=low_y && y<high_y && z>=low_z && z<high_z) 
								{
								nClass1=nClass2=0;

								//for(z2=std::max(low_z,z-1); z2<std::min(high_z,z+2); z2++)
								//	{
								//	for(y2=std::max(low_y,y-1); y2<std::min(high_y,y+2); y2++)
								//		{
								//		for(x2=std::max(low_x,x-1); x2<std::min(high_x,x+2); x2++)
								for(z2=std::max(0,z-1); z2<std::min(max_z,z+2); z2++)
									{
									for(y2=std::max(0,y-1); y2<std::min(max_y,y+2); y2++)
										{
										for(x2=std::max(0,x-1); x2<std::min(max_x,x+2); x2++)
											{
											k=x2+max_x*(y2+z2*max_y);
											if((*(output_iter+k))==class1)
												nClass1++;
											else if((*(output_iter+k))==class2)
												nClass2++;										
											}
										}
									}
								if(nClass1>nClass2)
									{
									(*(output_iter+j))=class1;
									changed=true;
									changedThis=true;
									}
								else if(nClass2>nClass1)
									{
									(*(output_iter+j))=class2;
									changed=true;
									changedThis=true;
									}
								//else if(nClass2>0 && nClass1>0)
								//	{
								//	(*(output_iter+j))=conflict;
								//	changed=true;
								//	changedThis=true;
								//	}
								}
							//else
							//	{
							//	(*(output_iter+j))=class1;
							//	changed=true;
							//	changedThis=true;
							//	}
							}
						}
					}
				}		        
			}
		//Clean up undefined
		for (i=number_of_voxels-1; i>=counts[0]; i--)//Skip lowest value
			{
			j=sorted_index[i];
			if((*(output_iter+j))==undef)
				{			    
				z=j/(max_x*max_y);
				rest=j-z*(max_x*max_y);
				y=rest/max_x;
				x=rest-y*max_x;		
				dims[0]=x;
				dims[1]=y;
				dims[2]=z;				
				if((allow_before && dims[y_direction]<no_internal_seeds[dims[z_direction]]) || ((!allow_before) && dims[y_direction]>no_internal_seeds[dims[z_direction]]))
					(*(output_iter+j))=class2;  
				else
					(*(output_iter+j))=class1;  
				}
			}  
		}
	delete[] counts;
	delete[] counts_cum;
	delete[] sorted_index;
	delete[] par_node;
	delete[] marked;
	delete[] npt_array;
	delete[] eq_neigh;

	//output->min_max_refresh();
	return output;
	}

template <class ELEMTYPE, int IMAGEDIM>
image_integer<ELEMTYPE, IMAGEDIM>* image_integer<ELEMTYPE, IMAGEDIM>::get_subvolume_from_slice_3D(int slice, int dir)
{
	cout<<"get_subvolume_from_region_3D..."<<endl;

	int usize=this->get_size_by_dim_and_dir(0,dir);
	int vsize=this->get_size_by_dim_and_dir(1,dir);
	pt_error::error_if_false(slice>=0 && slice<this->get_size_by_dim_and_dir(2,dir)," slice outside image dimensions in image_scalar<ELEMTYPE, IMAGEDIM>::get_subvolume_from_slice_3D",pt_error::debug);

	image_integer<ELEMTYPE, IMAGEDIM>* res;
	if (dir==0) {
		res = new image_integer<ELEMTYPE, IMAGEDIM>(1, usize, vsize);
		res->set_parameters(this);
		res->set_origin(this->get_physical_pos_for_voxel(slice,0,0));
	} else if (dir==1) {
		res = new image_integer<ELEMTYPE, IMAGEDIM>(vsize, 1, usize);
		res->set_parameters(this);
		res->set_origin(this->get_physical_pos_for_voxel(0,slice,0));
	} else {
		res = new image_integer<ELEMTYPE, IMAGEDIM>(usize, vsize, 1);
		res->set_parameters(this);
		res->set_origin(this->get_physical_pos_for_voxel(0,0,slice));
	}

	for (int u=0; u<usize; u++){
		for (int v=0; v<vsize; v++){
			res->set_voxel_by_dir(u,v,0, this->get_voxel_by_dir(u,v,slice,dir), dir);
		}
	}
	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
image_integer<ELEMTYPE, IMAGEDIM>* image_integer<ELEMTYPE, IMAGEDIM>::get_subvolume_from_region_3D(Vector3Dint vox_pos, Vector3Dint vox_size)
{
	return get_subvolume_from_region_3D(vox_pos[0],vox_pos[1],vox_pos[2],vox_pos[0]+vox_size[0],vox_pos[1]+vox_size[1],vox_pos[2]+vox_size[2]);
}

template <class ELEMTYPE, int IMAGEDIM>
image_integer<ELEMTYPE, IMAGEDIM>* image_integer<ELEMTYPE, IMAGEDIM>::get_subvolume_from_region_3D(int x1, int y1, int z1, int x2, int y2, int z2)
{
	//cout<<"image_integer-get_subvolume_from_region_3D..."<<endl;

	int nx=this->nx();
	int ny=this->ny();
	int nz=this->nz();
	x1 = max(x1,0);	x2 = min(x2,nx-1);
	y1 = max(y1,0);	y2 = min(y2,ny-1);
	z1 = max(z1,0);	z2 = min(z2,nz-1);

	image_integer<ELEMTYPE, IMAGEDIM>* res = new image_integer<ELEMTYPE, IMAGEDIM>(x2-x1+1, y2-y1+1, z2-z1+1);
	res->set_parameters(this);
	res->set_origin(this->get_physical_pos_for_voxel(x1,y1,z1));

	for (int z=z1, res_z=0; z<=z2; z++, res_z++){
		for (int y=y1, res_y=0; y<=y2; y++,res_y++){
			for (int x=x1, res_x=0; x<=x2; x++,res_x++){
				res->set_voxel(res_x,res_y,res_z, this->get_voxel(x,y,z));
			}
		}
	}
	return res;
}

template <class ELEMTYPE, int IMAGEDIM>
bool image_integer<ELEMTYPE, IMAGEDIM>::is_value_in_image(ELEMTYPE value)
{
	typename image_storage<ELEMTYPE >::iterator iter = this->begin();
    while (iter != this->end())
        {
        if (*iter==value)
			return true;
		++iter;
        }
	return false;
}

template <class ELEMTYPE, int IMAGEDIM>
bool image_integer<ELEMTYPE, IMAGEDIM>::is_value_in_slice_3D(ELEMTYPE value, int slice, int dir)
{
	int usize = this->get_size_by_dim_and_dir(0,dir);
	int vsize = this->get_size_by_dim_and_dir(1,dir);
	for (int u=0; u<usize; u++) {
		for (int v=0; v<vsize; v++) {
			if (this->get_voxel_by_dir(u,v,slice,dir)==value)
				return true;
		}
	}
	return false;
}


template <class ELEMTYPE, int IMAGEDIM>
int image_integer<ELEMTYPE, IMAGEDIM>::findNode(int e, int* par_node)
	{
		if(par_node[e]!=e)
			return findNode(par_node[e], par_node);
		else
			return e;
	}
	
template <class ELEMTYPE, int IMAGEDIM>
int image_integer<ELEMTYPE, IMAGEDIM>::mergeNodes(int e1, int e2, int* par_node)
	{
		int res;
		if((*(this->begin()+e1))==(*(this->begin()+e2)))
		{
			res=std::max(e1,e2);
			par_node[std::min(e1,e2)]=res;
		}
		else if((*(this->begin()+e1))>(*(this->begin()+e2)))
		{
			par_node[e1]=e2;
			res=e2;			
		}
		else
		{
			par_node[e2]=e1;
			res=e1;		
		}
		return res;
	}

template <class ELEMTYPE, int IMAGEDIM>
void image_integer<ELEMTYPE, IMAGEDIM>::markRecursive(int m, int* par_node, bool* marked)
	{
		if((*(this->begin()+m))>this->get_min())
		{
			if(!marked[m])
			{
				marked[m]=true;
				if(par_node[m]!=m)
					markRecursive(par_node[m], par_node, marked);
			}
		}
	}

template <class ELEMTYPE, int IMAGEDIM>
ELEMTYPE image_integer<ELEMTYPE, IMAGEDIM>::getSeedLevel(int m, int* par_node, bool* marked)
	{
		if((*(this->begin()+m))>this->get_min())
		{
			if(marked[m])
				return *(this->begin()+m);
			else if(par_node[m]!=m)
				return getSeedLevel(par_node[m], par_node, marked);
			else
				return 0;
		}
		else
			return 0;
	}

template <class ELEMTYPE, int IMAGEDIM>
string image_integer<ELEMTYPE, IMAGEDIM>::resolve_tooltip()
{
	return "image_integer\n" + resolve_tooltip_image_integer();
}

template <class ELEMTYPE, int IMAGEDIM>
string image_integer<ELEMTYPE, IMAGEDIM>::resolve_tooltip_image_integer()
{
	return this->resolve_tooltip_image_scalar();
}

template <class ELEMTYPE, int IMAGEDIM>
std::vector<HistoPair> image_integer<ELEMTYPE, IMAGEDIM>::get_distribution()
	{
	this->min_max_refresh();
	typename image_storage<ELEMTYPE >::iterator iter = this->begin();
    
	ELEMTYPE min_val=this->get_min();
	ELEMTYPE max_val=this->get_max();		
    int distinct_element_count = max_val - min_val + 1;
    int* counts = new int[distinct_element_count];
	memset(counts, 0, sizeof(int)*(1+max_val-min_val));
    while (iter != this->end()) //images are same size and should necessarily end at the same time
        {
        counts[(*iter)-min_val]++;
        ++iter;
        }

	ELEMTYPE i;
    std::vector<HistoPair> res;
	HistoPair p;        
    for(i=min_val; i<=max_val; i++)
		{
		if(counts[i-min_val]>0)
			{
			p.first=i;
			p.second=counts[i-min_val];
			res.push_back(p);
			}
		}

	delete[] counts;

	return res;
	}

#endif

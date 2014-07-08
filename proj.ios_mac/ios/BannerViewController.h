//
//  BannerViewController.h
//  FlappyMario
//
//  Created by Luca Diolaiuti on 23/06/14.
//
//


#import <UIKit/UIKit.h>

extern NSString * const BannerViewActionWillBegin;
extern NSString * const BannerViewActionDidFinish;

@interface BannerViewController : UIViewController

- (instancetype)initWithContentViewController:(UIViewController *)contentController;

@end

//
//  BannerViewController.m
//  FlappyMario
//
//  Created by Luca Diolaiuti on 23/06/14.
//
//


#import "BannerViewController.h"
#import "GADBannerView.h"

NSString * const BannerViewActionWillBegin = @"BannerViewActionWillBegin";
NSString * const BannerViewActionDidFinish = @"BannerViewActionDidFinish";

@interface BannerViewController () <GADBannerViewDelegate>

@end

@implementation BannerViewController {
    GADBannerView *_bannerView;
    UIViewController *_contentController;
    Boolean _bannerLoaded;
}

- (instancetype)initWithContentViewController:(UIViewController *)contentController
{
    self = [super init];
    if (self != nil) {
        _bannerView = [[GADBannerView alloc] initWithAdSize:kGADAdSizeSmartBannerPortrait];
        // Mediation ID or Publisher ID
        _bannerView.adUnitID = @"ca-app-pub-6602706598360211/4796714280";
        _bannerView.delegate = self;
        _contentController = contentController;
        _bannerLoaded = NO;
    }
    return self;
}

- (void)loadView
{
    UIView *contentView = [[UIView alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    [self addChildViewController:_contentController];
    [contentView addSubview:_contentController.view];
    [_contentController didMoveToParentViewController:self];
    [contentView addSubview:_bannerView];
    self.view = contentView;
}

#if __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_6_0
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return [_contentController shouldAutorotateToInterfaceOrientation:interfaceOrientation];
}
#endif

- (UIInterfaceOrientation)preferredInterfaceOrientationForPresentation
{
    return [_contentController preferredInterfaceOrientationForPresentation];
}

- (NSUInteger)supportedInterfaceOrientations
{
    return [_contentController supportedInterfaceOrientations];
}

// For animation
- (void)viewDidLayoutSubviews
{
    CGRect contentFrame = self.view.bounds;
    CGRect bannerFrame = CGRectZero;
#if __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_6_0
    bannerFrame = _bannerView.frame;
#else
    bannerFrame.size = [_bannerView sizeThatFits:contentFrame.size];
#endif
    
    bannerFrame.origin.x = (contentFrame.size.width - bannerFrame.size.width) / 2;
    
    //Bottom Screen
    if (_bannerLoaded) {
        //contentFrame.size.height -= bannerFrame.size.height;
        bannerFrame.origin.y = contentFrame.size.height - bannerFrame.size.height;
    } else {
        bannerFrame.origin.y = contentFrame.size.height;
    }
    //Top Screen
    //if (_bannerLoaded) {
    //    bannerFrame.origin.y = 0;
    //} else {
    //    bannerFrame.origin.y = -bannerFrame.size.height;
    //}
    
    _contentController.view.frame = contentFrame;
    _bannerView.frame = bannerFrame;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    _bannerView.rootViewController = self;
    [self.view addSubview:_bannerView];
    
    GADRequest *request = [GADRequest request];
    // For testing
    request.testDevices = [NSArray arrayWithObjects:@"ca-app-pub-6602706598360211/4796714280", nil];
    [_bannerView loadRequest:request];
}

- (void)adViewDidReceiveAd:(GADBannerView *)bannerView
{
    NSLog(@"adViewDidReceiveAd");
    _bannerLoaded = YES;
    [UIView animateWithDuration:0.25 animations:^{
        [self.view setNeedsLayout];
        [self.view layoutIfNeeded];
    }];
}

- (void)adView:(GADBannerView *)view didFailToReceiveAdWithError:(GADRequestError *)error
{
    NSLog(@"adView didFailToReceiveAdWithError");
    _bannerLoaded = NO;
    [UIView animateWithDuration:0.25 animations:^{
        [self.view setNeedsLayout];
        [self.view layoutIfNeeded];
    }];
}

- (void)adViewWillPresentScreen:(GADBannerView *)bannerView
{
    [[NSNotificationCenter defaultCenter] postNotificationName:BannerViewActionWillBegin object:self];
}

- (void)adViewWillDismissScreen:(GADBannerView *)bannerView
{
    [[NSNotificationCenter defaultCenter] postNotificationName:BannerViewActionDidFinish object:self];
}

- (void)dealloc {
    _bannerView.delegate = nil;
    [_bannerView release];
    [super dealloc];
}

@end